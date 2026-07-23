/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PTY surface — freestanding soft fill-in.
 * Soft deepen: prefer /dev/ptmx; else socketpair bidirectional soft PTY;
 * per-fd pts name table; login_tty setsid + stdio dup.
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 0
#endif
#ifndef AF_UNIX
#define AF_UNIX 1
#endif
#ifndef SOCK_STREAM
#define SOCK_STREAM 1
#endif

#define PTS_TAB 16
#define PTS_NAME 32

struct pts_ent {
	int nFd;
	int nSerial;
	char aName[PTS_NAME];
};

static int g_nPtsSerial;
static struct pts_ent g_aPts[PTS_TAB];

int openpty(int *pAmaster, int *pAslave, char *szName, const void *pTermp,
	    const void *pWinp);
int forkpty(int *pAmaster, char *szName, const void *pTermp,
	    const void *pWinp);
int login_tty(int nFd);

static void
itoa_dec(char *sz, size_t cb, int nVal)
{
	char aDig[12];
	int d = 0;
	size_t o = 0;
	int v = nVal;

	if (cb == 0) {
		return;
	}
	if (v < 0) {
		v = 0;
	}
	if (v == 0) {
		aDig[d++] = '0';
	} else {
		while (v > 0 && d < 11) {
			aDig[d++] = (char)('0' + (v % 10));
			v /= 10;
		}
	}
	while (d > 0 && o + 1 < cb) {
		sz[o++] = aDig[--d];
	}
	sz[o] = '\0';
}

static void
fmt_pts_name(char *sz, size_t cb, int nSerial)
{
	const char *pref = "/dev/pts/";
	size_t k = 0;

	while (pref[k] != '\0' && k + 1 < cb) {
		sz[k] = pref[k];
		k++;
	}
	itoa_dec(sz + k, (k < cb) ? cb - k : 0, nSerial);
}

static void
pts_register(int nFd, int nSerial, const char *szName)
{
	int i;
	int nFree = -1;

	if (nFd < 0) {
		return;
	}
	for (i = 0; i < PTS_TAB; i++) {
		if (g_aPts[i].nFd == nFd) {
			g_aPts[i].nSerial = nSerial;
			if (szName != NULL) {
				size_t j;

				for (j = 0; szName[j] != '\0' &&
					    j + 1 < PTS_NAME;
				     j++) {
					g_aPts[i].aName[j] = szName[j];
				}
				g_aPts[i].aName[j] = '\0';
			}
			return;
		}
		if (g_aPts[i].nFd == 0 && g_aPts[i].aName[0] == '\0' &&
		    nFree < 0) {
			nFree = i;
		}
	}
	if (nFree < 0) {
		nFree = nSerial % PTS_TAB;
	}
	g_aPts[nFree].nFd = nFd;
	g_aPts[nFree].nSerial = nSerial;
	if (szName != NULL) {
		size_t j;

		for (j = 0; szName[j] != '\0' && j + 1 < PTS_NAME; j++) {
			g_aPts[nFree].aName[j] = szName[j];
		}
		g_aPts[nFree].aName[j] = '\0';
	} else {
		fmt_pts_name(g_aPts[nFree].aName, PTS_NAME, nSerial);
	}
}

static const char *
pts_lookup(int nFd)
{
	int i;

	for (i = 0; i < PTS_TAB; i++) {
		if (g_aPts[i].nFd == nFd && g_aPts[i].aName[0] != '\0') {
			return g_aPts[i].aName;
		}
	}
	return NULL;
}

static void
copy_name(char *szDst, const char *szSrc, size_t cbMax)
{
	size_t i;
	size_t lim;

	if (szDst == NULL) {
		return;
	}
	/* Soft openpty name buffer historically 32-ish */
	lim = (cbMax == 0 || cbMax > 32) ? 32 : cbMax;
	if (szSrc == NULL) {
		szDst[0] = '\0';
		return;
	}
	for (i = 0; szSrc[i] != '\0' && i + 1 < lim; i++) {
		szDst[i] = szSrc[i];
	}
	szDst[i] = '\0';
}

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
						pts_register(m, 0, nm);
						pts_register(s, 0, nm);
						if (szName != NULL) {
							copy_name(szName, nm,
								  32);
						}
						return 0;
					}
				}
			}
			(void)close(m);
		}
	}
	/*
	 * Soft PTY: bidirectional socketpair (full-duplex). Fall back to
	 * pipe when socketpair is unavailable.
	 */
	n = ++g_nPtsSerial;
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, aFd) != 0) {
		if (pipe(aFd) != 0) {
			return -1;
		}
	}
	*pAmaster = aFd[0];
	*pAslave = aFd[1];
	{
		char aTmp[PTS_NAME];

		fmt_pts_name(aTmp, sizeof(aTmp), n);
		pts_register(aFd[0], n, aTmp);
		pts_register(aFd[1], n, aTmp);
		if (szName != NULL) {
			copy_name(szName, aTmp, 32);
		}
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
	/*
	 * Soft: TIOCSCTTY not required on freestanding; stdio dup is enough
	 * for shell/session bring-up.
	 */
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
	int aFd[2];
	int n;

	m = open("/dev/ptmx", nFlags);
	if (m >= 0) {
		return m;
	}
	/* Soft: socketpair end as master-shaped fd */
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, aFd) == 0) {
		(void)close(aFd[1]);
		n = ++g_nPtsSerial;
		{
			char aTmp[PTS_NAME];

			fmt_pts_name(aTmp, sizeof(aTmp), n);
			pts_register(aFd[0], n, aTmp);
		}
		errno = 0;
		return aFd[0];
	}
	if (pipe(aFd) != 0) {
		return -1;
	}
	(void)close(aFd[1]);
	n = ++g_nPtsSerial;
	{
		char aTmp[PTS_NAME];

		fmt_pts_name(aTmp, sizeof(aTmp), n);
		pts_register(aFd[0], n, aTmp);
	}
	errno = 0;
	return aFd[0];
}

int
grantpt(int nFd)
{
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
	static char aName[PTS_NAME];
	const char *p;

	if (nFd < 0) {
		errno = EBADF;
		return NULL;
	}
	p = pts_lookup(nFd);
	if (p != NULL) {
		copy_name(aName, p, sizeof(aName));
		errno = 0;
		return aName;
	}
	/* Soft fixed name when fd not registered. */
	fmt_pts_name(aName, sizeof(aName), 0);
	errno = 0;
	return aName;
}

int
ptsname_r(int nFd, char *szBuf, size_t cb)
{
	char *p;
	size_t n;

	if (szBuf == NULL || cb == 0) {
		errno = EINVAL;
		return -1;
	}
	p = ptsname(nFd);
	if (p == NULL) {
		return -1;
	}
	n = strlen(p);
	if (n + 1 > cb) {
		errno = ERANGE;
		return -1;
	}
	memcpy(szBuf, p, n + 1);
	return 0;
}
