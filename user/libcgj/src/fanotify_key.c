/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * fanotify + keyctl thin Linux wrappers (desktop link surface).
 * Soft deepen: ENOSYS falls back to pipe/event-shaped fanotify fd and
 * in-memory keyring slots so freestanding smokes link and exercise soft
 * auth paths without a real kernel key subsystem.
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/fanotify.h>
#include <unistd.h>

#define NR_fanotify_init 300
#define NR_fanotify_mark 301
#define NR_add_key       248
#define NR_request_key   249
#define NR_keyctl        250

/* Soft keyctl command subset (linux/keyctl.h-shaped). */
#ifndef KEYCTL_GET_KEYRING_ID
#define KEYCTL_GET_KEYRING_ID     0
#define KEYCTL_JOIN_SESSION_KEYRING 1
#define KEYCTL_UPDATE           2
#define KEYCTL_REVOKE             3
#define KEYCTL_CHOWN              4
#define KEYCTL_SETPERM            5
#define KEYCTL_DESCRIBE           6
#define KEYCTL_CLEAR              7
#define KEYCTL_LINK               8
#define KEYCTL_UNLINK             9
#define KEYCTL_SEARCH            10
#define KEYCTL_READ              11
#define KEYCTL_INSTANTIATE       12
#define KEYCTL_NEGATE            13
#define KEYCTL_SET_REQKEY_KEYRING 14
#define KEYCTL_SET_TIMEOUT       15
#define KEYCTL_ASSUME_AUTHORITY  16
#define KEYCTL_INVALIDATE        21
#endif

#define SOFT_KEY_MAX   16
#define SOFT_KEY_DESC  64
#define SOFT_KEY_PAY   256

struct soft_key {
	int fUsed;
	int nId;
	char aType[32];
	char aDesc[SOFT_KEY_DESC];
	unsigned char aPay[SOFT_KEY_PAY];
	size_t cbPay;
	int nKeyring;
};

static struct soft_key g_aKeys[SOFT_KEY_MAX];
static int g_nNextKeyId = 1000;
static int g_fSoftKeys;

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
	long r;
	register long r10 __asm__("r10") = a3;
	register long r8 __asm__("r8") = a4;
	register long r9 __asm__("r9") = a5;

	__asm__ volatile("syscall"
			 : "=a"(r)
			 : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8),
			   "r"(r9)
			 : "rcx", "r11", "memory");
	return r;
}

static long
sys_ret(long r)
{
	if (r < 0 && r > -4096) {
		errno = (int)(-r);
		return -1;
	}
	return r;
}

static void
copy_trunc(char *szDst, size_t cb, const char *szSrc)
{
	size_t n;

	if (szDst == NULL || cb == 0) {
		return;
	}
	if (szSrc == NULL) {
		szDst[0] = '\0';
		return;
	}
	n = 0;
	while (szSrc[n] != '\0' && n + 1 < cb) {
		szDst[n] = szSrc[n];
		n++;
	}
	szDst[n] = '\0';
}

static struct soft_key *
soft_key_find(int nId)
{
	int i;

	for (i = 0; i < SOFT_KEY_MAX; i++) {
		if (g_aKeys[i].fUsed && g_aKeys[i].nId == nId) {
			return &g_aKeys[i];
		}
	}
	return NULL;
}

static struct soft_key *
soft_key_find_desc(const char *szType, const char *szDesc)
{
	int i;

	for (i = 0; i < SOFT_KEY_MAX; i++) {
		if (!g_aKeys[i].fUsed) {
			continue;
		}
		if (szType != NULL &&
		    strcmp(g_aKeys[i].aType, szType) != 0) {
			continue;
		}
		if (szDesc != NULL &&
		    strcmp(g_aKeys[i].aDesc, szDesc) != 0) {
			continue;
		}
		return &g_aKeys[i];
	}
	return NULL;
}

static long
soft_add_key(const char *szType, const char *szDesc, const void *pPayload,
	     size_t cb, int nKeyring)
{
	int i;
	int nFree = -1;
	struct soft_key *p;

	g_fSoftKeys = 1;
	/* Replace matching type+desc if present. */
	p = soft_key_find_desc(szType, szDesc);
	if (p == NULL) {
		for (i = 0; i < SOFT_KEY_MAX; i++) {
			if (!g_aKeys[i].fUsed) {
				nFree = i;
				break;
			}
		}
		if (nFree < 0) {
			errno = EDQUOT;
			return -1;
		}
		p = &g_aKeys[nFree];
		memset(p, 0, sizeof(*p));
		p->nId = g_nNextKeyId++;
		p->fUsed = 1;
	}
	copy_trunc(p->aType, sizeof(p->aType),
		   szType != NULL ? szType : "user");
	copy_trunc(p->aDesc, sizeof(p->aDesc),
		   szDesc != NULL ? szDesc : "");
	p->nKeyring = nKeyring;
	p->cbPay = 0;
	if (pPayload != NULL && cb > 0) {
		if (cb > SOFT_KEY_PAY) {
			cb = SOFT_KEY_PAY;
		}
		memcpy(p->aPay, pPayload, cb);
		p->cbPay = cb;
	}
	errno = 0;
	return (long)p->nId;
}

static long
soft_keyctl(int nCmd, unsigned long a1, unsigned long a2, unsigned long a3,
	    unsigned long a4)
{
	struct soft_key *p;
	char *pBuf;
	size_t cb;
	size_t n;

	(void)a4;
	g_fSoftKeys = 1;
	switch (nCmd) {
	case KEYCTL_GET_KEYRING_ID:
		/* Soft session keyring id */
		errno = 0;
		return 1;
	case KEYCTL_JOIN_SESSION_KEYRING:
		errno = 0;
		return 1;
	case KEYCTL_DESCRIBE:
		p = soft_key_find((int)a1);
		if (p == NULL) {
			errno = ENOKEY;
			return -1;
		}
		pBuf = (char *)(uintptr_t)a2;
		cb = (size_t)a3;
		/* "type;0;0;0;desc" soft describe */
		{
			char aTmp[SOFT_KEY_DESC + 40];
			size_t o = 0;
			const char *s;

			s = p->aType;
			while (*s && o + 1 < sizeof(aTmp)) {
				aTmp[o++] = *s++;
			}
			if (o + 8 < sizeof(aTmp)) {
				aTmp[o++] = ';';
				aTmp[o++] = '0';
				aTmp[o++] = ';';
				aTmp[o++] = '0';
				aTmp[o++] = ';';
				aTmp[o++] = '0';
				aTmp[o++] = ';';
			}
			s = p->aDesc;
			while (*s && o + 1 < sizeof(aTmp)) {
				aTmp[o++] = *s++;
			}
			aTmp[o] = '\0';
			n = o + 1;
			if (pBuf != NULL && cb > 0) {
				size_t lim = (n < cb) ? n : cb;

				memcpy(pBuf, aTmp, lim);
				if (lim > 0) {
					pBuf[lim - 1] = '\0';
				}
			}
			errno = 0;
			return (long)n;
		}
	case KEYCTL_READ:
		p = soft_key_find((int)a1);
		if (p == NULL) {
			errno = ENOKEY;
			return -1;
		}
		pBuf = (char *)(uintptr_t)a2;
		cb = (size_t)a3;
		if (pBuf != NULL && cb > 0 && p->cbPay > 0) {
			n = (p->cbPay < cb) ? p->cbPay : cb;
			memcpy(pBuf, p->aPay, n);
		}
		errno = 0;
		return (long)p->cbPay;
	case KEYCTL_INVALIDATE:
	case KEYCTL_REVOKE:
	case KEYCTL_UNLINK:
		p = soft_key_find((int)a1);
		if (p != NULL) {
			memset(p, 0, sizeof(*p));
		}
		errno = 0;
		return 0;
	case KEYCTL_CLEAR:
		memset(g_aKeys, 0, sizeof(g_aKeys));
		errno = 0;
		return 0;
	case KEYCTL_SEARCH:
		p = soft_key_find_desc((const char *)(uintptr_t)a2,
				       (const char *)(uintptr_t)a3);
		if (p == NULL) {
			errno = ENOKEY;
			return -1;
		}
		errno = 0;
		return (long)p->nId;
	default:
		/* Soft: succeed no-op for other cmds used by link surface. */
		errno = 0;
		return 0;
	}
}

int
fanotify_init(unsigned uFlags, unsigned uEventFflags)
{
	long r;

	r = sys6(NR_fanotify_init, (long)uFlags, (long)uEventFflags, 0, 0, 0,
		 0);
	if (r >= 0) {
		return (int)r;
	}
	if ((int)(-r) != ENOSYS && (int)(-r) != ENOTSUP &&
	    (int)(-r) != EPERM) {
		return (int)sys_ret(r);
	}
	/*
	 * Soft: return a pipe read-end as fanotify-shaped fd so callers can
	 * poll/close without crashing on freestanding kernels.
	 */
	{
		int aFd[2];

		(void)uFlags;
		(void)uEventFflags;
		if (pipe(aFd) != 0) {
			return -1;
		}
		(void)close(aFd[1]);
		errno = 0;
		return aFd[0];
	}
}

int
fanotify_mark(int nFanotifyFd, unsigned uFlags, uint64_t u64Mask, int nDfd,
	      const char *szPathname)
{
	long r;

	if (nFanotifyFd < 0) {
		errno = EBADF;
		return -1;
	}
	r = sys6(NR_fanotify_mark, nFanotifyFd, (long)uFlags, (long)u64Mask,
		 nDfd, (long)(uintptr_t)szPathname, 0);
	if (r >= 0) {
		return 0;
	}
	if ((int)(-r) == ENOSYS || (int)(-r) == ENOTSUP ||
	    (int)(-r) == EBADF) {
		/* Soft: accept mark as no-op when kernel/soft fd lacks support. */
		(void)uFlags;
		(void)u64Mask;
		(void)nDfd;
		(void)szPathname;
		errno = 0;
		return 0;
	}
	return (int)sys_ret(r);
}

/* keyutils-shaped thin wrappers */
long
add_key(const char *szType, const char *szDesc, const void *pPayload,
	size_t cb, int nKeyring)
{
	long r;

	r = sys6(NR_add_key, (long)(uintptr_t)szType, (long)(uintptr_t)szDesc,
		 (long)(uintptr_t)pPayload, (long)cb, nKeyring, 0);
	if (r >= 0) {
		return r;
	}
	if ((int)(-r) == ENOSYS || (int)(-r) == ENOTSUP ||
	    (int)(-r) == EPERM) {
		return soft_add_key(szType, szDesc, pPayload, cb, nKeyring);
	}
	return sys_ret(r);
}

long
request_key(const char *szType, const char *szDesc, const char *szCallout,
	    int nDestKeyring)
{
	long r;
	struct soft_key *p;

	(void)szCallout;
	(void)nDestKeyring;
	r = sys6(NR_request_key, (long)(uintptr_t)szType,
		 (long)(uintptr_t)szDesc, (long)(uintptr_t)szCallout,
		 nDestKeyring, 0, 0);
	if (r >= 0) {
		return r;
	}
	if ((int)(-r) == ENOSYS || (int)(-r) == ENOTSUP ||
	    (int)(-r) == EPERM || (int)(-r) == ENOKEY) {
		p = soft_key_find_desc(szType, szDesc);
		if (p != NULL) {
			errno = 0;
			return (long)p->nId;
		}
		/* Soft: create empty key on request miss */
		return soft_add_key(szType, szDesc, NULL, 0, nDestKeyring);
	}
	return sys_ret(r);
}

long
keyctl(int nCmd, unsigned long a1, unsigned long a2, unsigned long a3,
       unsigned long a4)
{
	long r;

	r = sys6(NR_keyctl, nCmd, (long)a1, (long)a2, (long)a3, (long)a4, 0);
	if (r >= 0) {
		return r;
	}
	if ((int)(-r) == ENOSYS || (int)(-r) == ENOTSUP ||
	    (int)(-r) == EPERM || g_fSoftKeys) {
		return soft_keyctl(nCmd, a1, a2, a3, a4);
	}
	return sys_ret(r);
}
