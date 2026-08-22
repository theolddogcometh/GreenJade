/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PTY surface — Unix98 /dev/ptmx via posix_openpt (TIOCGPTN, TIOCSPTLCK);
 * socketpair fallback; ptsname ioctl then table; login_tty TIOCSCTTY + dup.
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
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
#ifndef TIOCGPTN
#define TIOCGPTN 0x80045430u /* Linux _IOR('T', 0x30, unsigned int) */
#endif
#ifndef TIOCSPTLCK
#define TIOCSPTLCK 0x40045431u /* Linux _IOW('T', 0x31, int) */
#endif

struct pts_ent {
	int fUsed;
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
char *ptsname(int nFd);
int __gj_pts_known(int nFd);
int __gj_pts_name(int nFd, char *szBuf, size_t cb);

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
	size_t j;

	if (nFd < 0) {
		return;
	}
	/* fUsed: fd 0 (login_tty stdin) is a valid pts, not a free slot. */
	for (i = 0; i < PTS_TAB; i++) {
		if (g_aPts[i].fUsed && g_aPts[i].nFd == nFd) {
			g_aPts[i].nSerial = nSerial;
			if (szName != NULL) {
				for (j = 0; szName[j] != '\0' &&
					    j + 1 < PTS_NAME;
				     j++) {
					g_aPts[i].aName[j] = szName[j];
				}
				g_aPts[i].aName[j] = '\0';
			}
			return;
		}
		if (!g_aPts[i].fUsed && nFree < 0) {
			nFree = i;
		}
	}
	if (nFree < 0) {
		nFree = nSerial % PTS_TAB;
		if (nFree < 0) {
			nFree = 0;
		}
	}
	g_aPts[nFree].fUsed = 1;
	g_aPts[nFree].nFd = nFd;
	g_aPts[nFree].nSerial = nSerial;
	if (szName != NULL) {
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

	if (nFd < 0) {
		return NULL;
	}
	for (i = 0; i < PTS_TAB; i++) {
		if (g_aPts[i].fUsed && g_aPts[i].nFd == nFd &&
		    g_aPts[i].aName[0] != '\0') {
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

static int
pts_resolve(int nFd, char *szBuf, size_t cb)
{
	unsigned uPn = 0;
	char aTmp[PTS_NAME];
	const char *szPts;
	size_t cbName;

	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	if (szBuf == NULL || cb == 0) {
		errno = EINVAL;
		return -1;
	}
	if (ioctl(nFd, TIOCGPTN, &uPn) == 0) {
		fmt_pts_name(aTmp, sizeof(aTmp), (int)uPn);
		pts_register(nFd, (int)uPn, aTmp);
		szPts = aTmp;
	} else {
		szPts = pts_lookup(nFd);
		if (szPts == NULL) {
			errno = ENOTTY;
			return -1;
		}
	}
	cbName = strlen(szPts);
	if (cbName + 1 > cb) {
		errno = ERANGE;
		return -1;
	}
	memcpy(szBuf, szPts, cbName + 1);
	return 0;
}

static void
pty_apply(int nSlave, const void *pTermp, const void *pWinp)
{
	if (nSlave < 0) {
		return;
	}
	if (pTermp != NULL) {
		(void)tcsetattr(nSlave, TCSANOW, (const struct termios *)pTermp);
	}
	if (pWinp != NULL) {
		(void)ioctl(nSlave, TIOCSWINSZ, pWinp);
	}
}

int
openpty(int *pAmaster, int *pAslave, char *szName, const void *pTermp,
	const void *pWinp)
{
	int aFd[2];
	int n;
	int nMaster;

	if (pAmaster == NULL || pAslave == NULL) {
		errno = EINVAL;
		return -1;
	}
	/* posix_openpt owns ptmx + TIOCGPTN; socketpair fallback below. */
	nMaster = posix_openpt(O_RDWR | O_NOCTTY | O_CLOEXEC);
	if (nMaster >= 0) {
		if (grantpt(nMaster) == 0 && unlockpt(nMaster) == 0) {
			char *szNm = ptsname(nMaster);
			int nSlave;

			if (szNm != NULL) {
				nSlave = open(szNm, O_RDWR | O_NOCTTY);
				if (nSlave >= 0) {
					unsigned uPn = 0;
					int nSerial = 0;

					*pAmaster = nMaster;
					*pAslave = nSlave;
					if (ioctl(nMaster, TIOCGPTN,
						  &uPn) == 0) {
						nSerial = (int)uPn;
					}
					pts_register(nSlave, nSerial, szNm);
					if (szName != NULL) {
						copy_name(szName, szNm, 32);
					}
					pty_apply(nSlave, pTermp, pWinp);
					return 0;
				}
			}
		}
		(void)close(nMaster);
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
	pty_apply(aFd[1], pTermp, pWinp);
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
	{
		int nSaved = errno;

		if (ioctl(nFd, TIOCSCTTY, 0) < 0 && errno != ENOTTY &&
		    errno != ENOSYS && errno != EINVAL && errno != EPERM) {
			return -1;
		}
		errno = nSaved;
	}
	if (dup2(nFd, STDIN_FILENO) < 0 || dup2(nFd, STDOUT_FILENO) < 0 ||
	    dup2(nFd, STDERR_FILENO) < 0) {
		return -1;
	}
	{
		char aName[PTS_NAME];

		if (pts_resolve(nFd, aName, sizeof(aName)) == 0) {
			pts_register(STDIN_FILENO, 0, aName);
			pts_register(STDOUT_FILENO, 0, aName);
			pts_register(STDERR_FILENO, 0, aName);
		}
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
	if (m < 0) {
		m = open("/dev/pts/ptmx", nFlags);
	}
	if (m >= 0) {
		char aTmp[PTS_NAME];
		unsigned uPn = 0;

		if (ioctl(m, TIOCGPTN, &uPn) == 0) {
			fmt_pts_name(aTmp, sizeof(aTmp), (int)uPn);
			pts_register(m, (int)uPn, aTmp);
		} else {
			n = ++g_nPtsSerial;
			fmt_pts_name(aTmp, sizeof(aTmp), n);
			pts_register(m, n, aTmp);
		}
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
	char *szNm;

	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	szNm = ptsname(nFd);
	if (szNm != NULL) {
		(void)chmod(szNm, 0620);
	}
	errno = 0;
	return 0;
}

int
unlockpt(int nFd)
{
	int nZero = 0;
	int nSaved;

	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	nSaved = errno;
	if (ioctl(nFd, TIOCSPTLCK, &nZero) == 0) {
		return 0;
	}
	if (errno == ENOTTY || errno == ENOSYS || errno == EINVAL ||
	    errno == EIO) {
		errno = nSaved;
		return 0;
	}
	return -1;
}

int
__gj_pts_known(int nFd)
{
	return pts_lookup(nFd) != NULL;
}

int
__gj_pts_name(int nFd, char *szBuf, size_t cb)
{
	const char *p;

	if (szBuf == NULL || cb == 0) {
		errno = EINVAL;
		return -1;
	}
	p = pts_lookup(nFd);
	if (p == NULL) {
		errno = ENOTTY;
		return -1;
	}
	copy_name(szBuf, p, cb);
	return 0;
}

char *
ptsname(int nFd)
{
	static char aName[PTS_NAME];

	if (pts_resolve(nFd, aName, sizeof(aName)) != 0) {
		return NULL;
	}
	return aName;
}

int
ptsname_r(int nFd, char *szBuf, size_t cb)
{
	return pts_resolve(nFd, szBuf, cb);
}

int
getpt(void)
{
	return posix_openpt(O_RDWR | O_NOCTTY);
}
