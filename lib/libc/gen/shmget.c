#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$Id$";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#if __STDC__
int shmget(key_t key, int size, int shmflg)
#else
int shmget(key, size, shmflg)
	key_t key;
	int size;
	int shmflg;
#endif
{
	return (shmsys(3, key, size, shmflg));
}
