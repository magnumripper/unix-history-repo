/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Adam S. Moskowitz of Menlo Consulting and Marciano Pitargue.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)cut.c	5.1 (Berkeley) %G%";
#endif /* not lint */

#include <limits.h>
#include <stdio.h>
#include <ctype.h>

int	cflag;
char	dchar;
int	dflag;
int	fflag;
int	sflag;

main(argc, argv)
	int argc;
	char **argv;
{
	extern char *optarg;
	extern int errno, optind;
	FILE *fp;
	int ch, (*fcn)(), c_cut(), f_cut();
	char *strerror();

	dchar = '\t';			/* default delimiter is \t */

	while ((ch = getopt(argc, argv, "c:d:f:s")) != EOF)
		switch(ch) {
		case 'c':
			fcn = c_cut;
			get_list(optarg);
			cflag = 1;
			break;
		case 'd':
			dchar = *optarg;
			dflag = 1;
			break;
		case 'f':
			get_list(optarg);
			fcn = f_cut;
			fflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (fflag) {
		if (cflag)
			usage();
	} else if (!cflag || dflag || sflag)
		usage();

	if (*argv)
		for (; *argv; ++argv) {
			if (!(fp = fopen(*argv, "r"))) {
				(void)fprintf(stderr,
				    "cut: %s: %s\n", *argv, strerror(errno));
				exit(1);
			}
			fcn(fp, *argv);
		}
	else
		fcn(stdin, "stdin");
	exit(0);
}

int autostart, autostop, maxval;

char positions[LINE_MAX + 1];

get_list(list)
	char *list;
{
	register char *pos;
	register int setautostart, start, stop;
	char *p, *strtok();

	/*
	 * set a byte in the positions array to indicate if a field or
	 * column is to be selected; use +1, it's 1-based, not 0-based.
	 * This parser is less restrictive than the Draft 9 POSIX spec.
	 * POSIX doesn't allow lists that aren't in increasing order or
	 * overlapping lists.  We also handle "-3-5" although there's no
	 * real reason too.
	 */
	for (; p = strtok(list, ", \t"); list = NULL) {
		setautostart = start = stop = 0;
		if (*p == '-') {
			++p;
			setautostart = 1;
		}
		if (isdigit(*p)) {
			start = stop = strtol(p, &p, 10);
			if (setautostart && start > autostart)
				autostart = start;
		}
		if (*p == '-') {
			if (isdigit(p[1]))
				stop = strtol(p + 1, &p, 10);
			if (*p == '-') {
				++p;
				if (!autostop || autostop > stop)
					autostop = stop;
			}
		}
		if (*p)
			badlist("illegal list value");
		if (!stop || !start)
			badlist("values may not include zero");
		if (stop > LINE_MAX) {
			/* positions used rather than allocate a new buffer */
			(void)sprintf(positions, "%d too large (max %d)",
			    stop, LINE_MAX);
			badlist(positions);
		}
		if (maxval < stop)
			maxval = stop;
		for (pos = positions + start; start++ <= stop; *pos++ = 1);
	}

	/* overlapping ranges */
	if (autostop && maxval > autostop)
		maxval = autostop;

	/* set autostart */
	if (autostart)
		memset(positions + 1, '1', autostart);
}

/* ARGSUSED */
c_cut(fp, fname)
	FILE *fp;
	char *fname;
{
	register int ch, col;
	register char *pos;

	for (;;) {
		pos = positions + 1;
		for (col = maxval; col; --col) {
			if ((ch = getc(fp)) == EOF)
				return;
			if (ch == '\n')
				break;
			if (*pos++)
				putchar(ch);
		}
		if (ch != '\n')
			if (autostop)
				while ((ch = getc(fp)) != EOF && ch != '\n')
					putchar(ch);
			else
				while ((ch = getc(fp)) != EOF && ch != '\n');
		putchar('\n');
	}
}

f_cut(fp, fname)
	FILE *fp;
	char *fname;
{
	register int ch, field, isdelim;
	register char *pos, *p, sep;
	int output;
	char lbuf[LINE_MAX + 1];

	for (sep = dchar, output = 0; fgets(lbuf, sizeof(lbuf), fp);) {
		for (isdelim = 0, p = lbuf;; ++p) {
			if (!(ch = *p)) {
				(void)fprintf(stderr,
				    "cut: %s: line too long.\n", fname);
				exit(1);
			}
			/* this should work if newline is delimiter */
			if (ch == sep)
				isdelim = 1;
			if (ch == '\n') {
				if (!isdelim && !sflag)
					(void)printf("%s", lbuf);
				break;
			}
		}
		if (!isdelim)
			continue;

		pos = positions + 1;
		for (field = maxval, p = lbuf; field; --field, ++pos) {
			if (*pos) {
				if (output++)
					putchar(sep);
				while ((ch = *p++) != '\n' && ch != sep)
					putchar(ch);
			} else
				while ((ch = *p++) != '\n' && ch != sep);
			if (ch == '\n')
				break;
		}
		if (ch != '\n')
			if (autostop) {
				if (output)
					putchar(sep);
				for (; (ch = *p) != '\n'; ++p)
					putchar(ch);
			} else
				for (; (ch = *p) != '\n'; ++p);
		putchar('\n');
	}
}

badlist(msg)
	char *msg;
{
	(void)fprintf(stderr, "cut: [-cf] list: %s.\n", msg);
	exit(1);
}

usage()
{
	(void)fprintf(stderr,
"usage:\tcut -c list [file1 ...]\n\tcut -f list [-s] [-d delim] [file ...]\n");
	exit(1);
}
