/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PTY surface — freestanding soft fill-in using pipe pairs when no
 * kernel /dev/ptmx. openpty returns connected master/slave fds;
 * ptsname reports /dev/pts/N soft path.
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

static int g_nPtsSerial;

int openpty(int *pAmaster, int *pAslave, char *szName, const void *pTermp,
	    const void *pWinp);
int forkpty(int *pAmaster, char *szName, const void *pTermp,
	    const void *pWinp);
int login_tty(int nFd);

int
openpty(int *pAmaster, int *pAslave, char *szName, const void *pTermp,
	const void *pWinp)
{
	int aFd[2];
	int n;

	(void)pTermp;
	(void)pWinp;
	if (pAmaster == NULL || pAslave == NULL) {
		errno = EINVAL;
		return -1;
	}
	/* Prefer real ptmx when the host/kernel provides it. */
	{
		int m = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_CLOEXEC);

		if (m >= 0) {
			if (grantpt(m) == 0 && unlockpt(m) == 0) {
				char *nm = ptsname(m);
				int s;

				if (nm != NULL) {
					s = open(nm, O_RDWR | O_NOCTTY);
					if (s >= 0) {
						*pAmaster = m;
						*pAslave = s;
						if (szName != NULL) {
							size_t i;

							for (i = 0;
							     nm[i] != '\0' &&
							     i < 31;
							     i++) {
								szName[i] =
								    nm[i];
							}
							szName[i] = '\0';
						}
						return 0;
					}
				}
			}
			(void)close(m);
		}
	}
	/* Soft PTY: bidirectional pipe pair (half-duplex bring-up). */
	if (pipe(aFd) != 0) {
		return -1;
	}
	*pAmaster = aFd[0];
	*pAslave = aFd[1];
	n = ++g_nPtsSerial;
	if (szName != NULL) {
		/* minimal itoa into /dev/pts/N */
		char aTmp[32];
		int k = 0;
		int v = n;
		char *p;
		const char *pref = "/dev/pts/";

		while (pref[k] != '\0') {
			aTmp[k] = pref[k];
			k++;
		}
		if (v == 0) {
			aTmp[k++] = '0';
		} else {
			char dig[12];
			int d = 0;

			while (v > 0 && d < 11) {
				dig[d++] = (char)('0' + (v % 10));
				v /= 10;
			}
			while (d > 0) {
				aTmp[k++] = dig[--d];
			}
		}
		aTmp[k] = '\0';
		p = aTmp;
		while (*p != '\0') {
			*szName++ = *p++;
		}
		*szName = '\0';
	}
	errno = 0;
	return 0;
}

int
forkpty(int *pAmaster, char *szName, const void *pTermp, const void *pWinp)
{
	int master = -1;
	int slave = -1;
	pid_t pid;

	if (openpty(&master, &slave, szName, pTermp, pWinp) != 0) {
		return -1;
	}
	pid = fork();
	if (pid < 0) {
		(void)close(master);
		(void)close(slave);
		return -1;
	}
	if (pid == 0) {
		(void)close(master);
		if (login_tty(slave) != 0) {
			_exit(127);
		}
		return 0; /* child */
	}
	(void)close(slave);
	if (pAmaster != NULL) {
		*pAmaster = master;
	} else {
		(void)close(master);
	}
	return (int)pid;
}

int
login_tty(int nFd)
{
	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	(void)setsid();
	if (dup2(nFd, STDIN_FILENO) < 0 || dup2(nFd, STDOUT_FILENO) < 0 ||
	    dup2(nFd, STDERR_FILENO) < 0) {
		return -1;
	}
	if (nFd > STDERR_FILENO) {
		(void)close(nFd);
	}
	return 0;
}

int
posix_openpt(int nFlags)
{
	int m;

	m = open("/dev/ptmx", nFlags);
	if (m >= 0) {
		return m;
	}
	/* Soft: return a pipe end as master-shaped fd */
	{
		int aFd[2];

		if (pipe(aFd) != 0) {
			return -1;
		}
		(void)close(aFd[1]);
		return aFd[0];
	}
}

int
grantpt(int nFd)
{
	(void)nFd;
	/* Soft: always succeed when fd is open. */
	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	return 0;
}

int
unlockpt(int nFd)
{
	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	return 0;
}

char *
ptsname(int nFd)
{
	static char aName[32];

	if (nFd < 0) {
		errno = EBADF;
		return NULL;
	}
	/* Soft fixed name; real systems use TIOCGPTN. */
	{
		const char *s = "/dev/pts/0";
		size_t i;

		for (i = 0; s[i] != '\0' && i + 1 < sizeof(aName); i++) {
			aName[i] = s[i];
		}
		aName[i] = '\0';
	}
	errno = 0;
	return aName;
}

int
ptsname_r(int nFd, char *szBuf, size_t cb)
{
	char *p;

	if (szBuf == NULL || cb == 0) {
		errno = EINVAL;
		return -1;
	}
	p = ptsname(nFd);
	if (p == NULL) {
		return -1;
	}
	if (strlen(p) + 1 > cb) {
		errno = ERANGE;
		return -1;
	}
	memcpy(szBuf, p, strlen(p) + 1);
	return 0;
}
