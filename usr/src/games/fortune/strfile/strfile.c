/* $Header: strfile.c,v 1.19 89/05/05 16:07:46 arnold Exp $ */

# include	<stdio.h>
# include	<ctype.h>
# include	<sys/types.h>
# include	<sys/param.h>
# include	"strfile.h"
# include	"random.h"

# ifndef MAXPATHLEN
# define	MAXPATHLEN	1024
# endif	/* MAXPATHLEN */

/*
 *	This program takes a file composed of strings seperated by
 * lines starting with two consecutive delimiting character (default
 * character is '%') and creates another file which consists of a table
 * describing the file (structure from "strfile.h"), a table of seek
 * pointers to the start of the strings, and the strings, each terinated
 * by a null byte.  Usage:
 *
 *	% strfile [ - ] [ -cC ] [ -sv ] [ -oir ] sourcefile [ datafile ]
 *
 *	- - Give a usage summary useful for jogging the memory
 *	c - Change delimiting character from '%' to 'C'
 *	s - Silent.  Give no summary of data processed at the end of
 *	    the run.
 *	v - Verbose.  Give summary of data processed.  (Default)
 *	o - order the strings in alphabetic order
 *	i - if ordering, ignore case 
 *	r - randomize the order of the strings
 *
 *		Ken Arnold	Sept. 7, 1978 --
 *
 *	Added method to indicate dividers.  A "%-" will cause the address
 * to be added to the structure in one of the pointer elements.
 *
 *		Ken Arnold	Nov., 1984 --
 *
 *	Added ordering options.
 */

# define	TRUE	1
# define	FALSE	0

# define	STORING_PTRS	(Oflag || Rflag)
# define	CHUNKSIZE	512

#ifdef lint
# define	ALWAYS	atoi("1")
#else
# define	ALWAYS	1
#endif
# define	ALLOC(ptr,sz)	if (ALWAYS) { \
			if (ptr == NULL) \
				ptr = malloc((unsigned int) (CHUNKSIZE * sizeof *ptr)); \
			else if (((sz) + 1) % CHUNKSIZE == 0) \
				ptr = realloc((void *) ptr, ((unsigned int) ((sz) + CHUNKSIZE) * sizeof *ptr)); \
			if (ptr == NULL) { \
				fprintf(stderr, "out of space\n"); \
				exit(1); \
			} \
		} else

#ifdef NO_VOID
# define	void	char
#endif

typedef struct {
	char	first;
	off_t	pos;
} STR;

char	*Infile		= NULL,		/* input file name */
	Outfile[MAXPATHLEN] = "",	/* output file name */
	Delimch		= '%',		/* delimiting character */
	*Usage[]	= {		/* usage summary */
       "usage:	strfile [ - ] [ -cC ] [ -sv ] [ -oir ] inputfile [ datafile ]",
       "	- - Give this usage summary",
       "	c - Replace delimiting character with 'C'",
       "	s - Silent.  Give no summary",
       "	v - Verbose.  Give summary.  (default)",
       "	o - order strings alphabetically",
       "	i - ignore case in ordering",
       "	r - randomize the order of the strings",
       "	Default \"datafile\" is inputfile.dat",
	};

int	Sflag		= FALSE;	/* silent run flag */
int	Oflag		= FALSE;	/* ordering flag */
int	Iflag		= FALSE;	/* ignore case flag */
int	Rflag		= FALSE;	/* randomize order flag */
int	Num_pts		= 0;		/* number of pointers/strings */

off_t	*Seekpts;

FILE	*Sort_1, *Sort_2;		/* pointers for sorting */

STRFILE	Tbl;				/* statistics table */

STR	*Firstch;			/* first chars of each string */

char	*fgets(), *strcpy(), *strcat();

void	*malloc(), *realloc();

/*
 * main:
 *	Drive the sucker.  There are two main modes -- either we store
 *	the seek pointers, if the table is to be sorted or randomized,
 *	or we write the pointer directly to the file, if we are to stay
 *	in file order.  If the former, we allocate and re-allocate in
 *	CHUNKSIZE blocks; if the latter, we just write each pointer,
 *	and then seek back to the beginning to write in the table.
 */
main(ac, av)
int	ac;
char	**av;
{
	register char		*sp, dc;
	register FILE		*inf, *outf;
	register off_t		last_off, length, pos;
	register int		first;
	register char		*nsp;
	register STR		*fp;
	static char		string[257];

	getargs(ac, av);		/* evalute arguments */
	dc = Delimch;
	if ((inf = fopen(Infile, "r")) == NULL) {
		perror(Infile);
		exit(-1);
	}

	if ((outf = fopen(Outfile, "w")) == NULL) {
		perror(Outfile);
		exit(-1);
	}
	if (!STORING_PTRS)
		(void) fseek(outf, sizeof Tbl, 0);

	/*
	 * Write the strings onto the file
	 */

	Tbl.str_longlen = 0;
	Tbl.str_shortlen = (unsigned int) 0xffffffff;
	Tbl.str_delim = dc;
	first = Oflag;
	add_offset(outf, ftell(inf));
	last_off = 0;
	do {
		if (Num_pts > 508)
			atoi("1");
		sp = fgets(string, 256, inf);
		if (sp == NULL || (sp[0] == dc && sp[1] == dc)) {
			pos = ftell(inf);
			add_offset(outf, pos);
			length = pos - last_off - strlen(sp);
			last_off = pos;
			if (Tbl.str_longlen < length)
				Tbl.str_longlen = length;
			if (Tbl.str_shortlen > length)
				Tbl.str_shortlen = length;
			first = Oflag;
		}
		else {
			if (first) {
				for (nsp = sp; !isalnum(*nsp); nsp++)
					continue;
				ALLOC(Firstch, Num_pts);
				fp = &Firstch[Num_pts - 1];
				if (Iflag && isupper(*nsp))
					fp->first = tolower(*nsp);
				else
					fp->first = *nsp;
				fp->pos = Seekpts[Num_pts - 1];
				first = FALSE;
			}
		}
	} while (sp != NULL);

	/*
	 * write the tables in
	 */

	(void) fclose(inf);
	Tbl.str_numstr = Num_pts - 1;

	if (Oflag)
		do_order();
	else if (Rflag)
		randomize();

	(void) fseek(outf, (off_t) 0, 0);
	(void) fwrite((char *) &Tbl, sizeof Tbl, 1, outf);
	if (STORING_PTRS)
		(void) fwrite((char *) Seekpts, sizeof *Seekpts, (int) Num_pts, outf);
	(void) fclose(outf);

	if (!Sflag) {
		printf("\"%s\" created\n", Outfile);
		if (Num_pts == 2)
			puts("There was 1 string");
		else
			printf("There were %u strings\n", Num_pts - 1);
		printf("Longest string: %u byte%s\n", Tbl.str_longlen,
		       Tbl.str_longlen == 1 ? "" : "s");
		printf("Shortest string: %u byte%s\n", Tbl.str_shortlen,
		       Tbl.str_shortlen == 1 ? "" : "s");
	}
	exit(0);
	/* NOTREACHED */
}

/*
 *	This routine evaluates arguments from the command line
 */
getargs(ac, av)
register int	ac;
register char	**av;
{
	register char	*sp;
	register int	i;
	register int	bad, j;

	bad = 0;
	for (i = 1; i < ac; i++)
		if (*av[i] == '-' && av[i][1]) {
			for (sp = &av[i][1]; *sp; sp++)
				switch (*sp) {
				  case 'c': /* new delimiting char */
					if ((Delimch = *++sp) == '\0') {
						--sp;
						Delimch = *av[++i];
					}
					if (!isascii(Delimch)) {
						printf("bad delimiting character: '\\%o\n'",
						       Delimch);
						bad++;
					}
					break;
				  case 's':	/* silent */
					Sflag++;
					break;
				  case 'v':	/* verbose */
					Sflag = 0;
					break;
				  case 'o':	/* order strings */
					Oflag++;
					break;
				  case 'i':	/* ignore case in ordering */
					Iflag++;
					break;
				  case 'r':	/* ignore case in ordering */
					Rflag++;
					break;
				  default:	/* unknown flag */
					bad++;
					printf("bad flag: '%c'\n", *sp);
					break;
				}
		}
		else if (*av[i] == '-') {
			for (j = 0; Usage[j]; j++)
				puts(Usage[j]);
			exit(0);
		}
		else if (Infile)
			(void) strcpy(Outfile, av[i]);
		else
			Infile = av[i];
	if (!Infile) {
		bad++;
		puts("No input file name");
	}
	if (*Outfile == '\0' && !bad) {
		(void) strcpy(Outfile, Infile);
		(void) strcat(Outfile, ".dat");
	}
	if (bad) {
		puts("use \"strfile -\" to get usage");
		exit(-1);
	}
}

/*
 * add_offset:
 *	Add an offset to the list, or write it out, as appropriate.
 */
add_offset(fp, off)
FILE	*fp;
off_t	off;
{
	if (!STORING_PTRS)
		fwrite(&off, 1, sizeof off, fp);
	else {
		ALLOC(Seekpts, Num_pts + 1);
		Seekpts[Num_pts] = off;
	}
	Num_pts++;
}

/*
 * do_order:
 *	Order the strings alphabetically (possibly ignoring case).
 */
do_order()
{
	register int	i;
	register off_t	*lp;
	register STR	*fp;
	extern int	cmp_str();

	Sort_1 = fopen(Infile, "r");
	Sort_2 = fopen(Infile, "r");
	qsort((char *) Firstch, (int) Tbl.str_numstr, sizeof *Firstch, cmp_str);
	i = Tbl.str_numstr;
	lp = Seekpts;
	fp = Firstch;
	while (i--)
		*lp++ = fp++->pos;
	(void) fclose(Sort_1);
	(void) fclose(Sort_2);
	Tbl.str_flags |= STR_ORDERED;
}

/*
 * cmp_str:
 *	Compare two strings in the file
 */
char *
unctrl(c)
char c;
{
	static char	buf[3];

	if (isprint(c)) {
		buf[0] = c;
		buf[1] = '\0';
	}
	else if (c == 0177) {
		buf[0] = '^';
		buf[1] = '?';
	}
	else {
		buf[0] = '^';
		buf[1] = c + 'A' - 1;
	}
	return buf;
}

cmp_str(p1, p2)
STR	*p1, *p2;
{
	register int	c1, c2;
	register int	n1, n2;

# define	SET_N(nf,ch)	(nf = (ch == '\n'))
# define	IS_END(ch,nf)	(ch == Delimch && nf)

	c1 = p1->first;
	c2 = p2->first;
	if (c1 != c2)
		return c1 - c2;

	(void) fseek(Sort_1, p1->pos, 0);
	(void) fseek(Sort_2, p2->pos, 0);

	n1 = FALSE;
	n2 = FALSE;
	while (!isalnum(c1 = getc(Sort_1)) && c1 != '\0')
		SET_N(n1, c1);
	while (!isalnum(c2 = getc(Sort_2)) && c2 != '\0')
		SET_N(n2, c2);

	while (!IS_END(c1, n1) && !IS_END(c2, n2)) {
		if (Iflag) {
			if (isupper(c1))
				c1 = tolower(c1);
			if (isupper(c2))
				c2 = tolower(c2);
		}
		if (c1 != c2)
			return c1 - c2;
		if (c1 == '\n' || c2 == '\n')
			atoi("1");
		SET_N(n1, c1);
		SET_N(n2, c2);
		c1 = getc(Sort_1);
		c2 = getc(Sort_2);
	}
	if (IS_END(c1, n1))
		c1 = 0;
	if (IS_END(c2, n2))
		c2 = 0;
	return c1 - c2;
}

/*
 * randomize:
 *	Randomize the order of the string table.  We must be careful
 *	not to randomize across delimiter boundaries.  All
 *	randomization is done within each block.
 */
randomize()
{
	register int	cnt, i;
	register off_t	tmp;
	register off_t	*sp;
	extern time_t	time();

	Tbl.str_flags |= STR_RANDOM;
	cnt = Tbl.str_numstr;

	/*
	 * move things around randomly
	 */

	for (sp = Seekpts; cnt > 0; cnt--, sp++) {
		i = rnd((long) cnt);
		tmp = sp[0];
		sp[0] = sp[i];
		sp[i] = tmp;
	}
}
