/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * getauxval — walk AT_ vector after envp on process stack (Linux x86_64).
 * Soft deepen: larger cache, success clears errno, freestanding fallbacks
 * when the stack auxv is unavailable (static CRT / early call).
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/auxv.h>
#include <unistd.h>

#ifndef AT_NULL
#define AT_NULL 0
#endif

/* Captured at first call via stack walk from environ */
enum { GJ_AUX_MAX = 96 };

static int            g_fAuxInit;
static int            g_fAuxHave; /* 1 if walk saw at least AT_NULL */
static unsigned long  g_aAux[GJ_AUX_MAX][2];
static int            g_nAux;

static void
auxv_capture_from_env(char **env)
{
	unsigned long *p;
	int n;

	if (env == NULL) {
		return;
	}
	/* walk past envp NULL terminator */
	while (*env != NULL) {
		env++;
	}
	env++;
	p = (unsigned long *)(void *)env;
	n = 0;
	while (n < GJ_AUX_MAX) {
		unsigned long uType = p[0];
		unsigned long uVal = p[1];

		g_aAux[n][0] = uType;
		g_aAux[n][1] = uVal;
		n++;
		if (uType == (unsigned long)AT_NULL) {
			g_fAuxHave = 1;
			break;
		}
		/*
		 * Soft sanity: absurd type tags mean we did not land on auxv
		 * (misaligned stack / missing vector). Abort capture.
		 */
		if (uType > 0x10000ul) {
			n = 0;
			g_fAuxHave = 0;
			break;
		}
		p += 2;
	}
	g_nAux = n;
}

static void
auxv_init(void)
{
	if (g_fAuxInit) {
		return;
	}
	g_fAuxInit = 1;
	g_fAuxHave = 0;
	g_nAux = 0;
	auxv_capture_from_env(environ);
}

/* Soft defaults when stack auxv is missing (freestanding bring-up). */
static int
auxv_soft_default(unsigned long uType, unsigned long *puOut)
{
	switch (uType) {
	case AT_NULL:
		*puOut = 0;
		return 1;
	case AT_PAGESZ:
		/* Prefer live page size when available. */
		{
			long n = sysconf(_SC_PAGESIZE);

			if (n <= 0) {
				n = (long)getpagesize();
			}
			if (n <= 0) {
				n = 4096;
			}
			*puOut = (unsigned long)n;
			return 1;
		}
	case AT_CLKTCK: {
		long n = sysconf(_SC_CLK_TCK);

		if (n <= 0) {
			n = 100;
		}
		*puOut = (unsigned long)n;
		return 1;
	}
	case AT_UID:
		*puOut = (unsigned long)getuid();
		return 1;
	case AT_EUID:
		*puOut = (unsigned long)geteuid();
		return 1;
	case AT_GID:
		*puOut = (unsigned long)getgid();
		return 1;
	case AT_EGID:
		*puOut = (unsigned long)getegid();
		return 1;
	case AT_SECURE:
		/* Soft: unprivileged unless euid/uid differ. */
		*puOut = (getuid() != geteuid() || getgid() != getegid())
				 ? 1ul
				 : 0ul;
		return 1;
	case AT_HWCAP:
	case AT_HWCAP2:
	case AT_FLAGS:
		*puOut = 0;
		return 1;
	default:
		return 0;
	}
}

unsigned long
getauxval(unsigned long uType)
{
	int i;
	unsigned long uSoft;

	auxv_init();

	/* Prefer real vector when capture succeeded. */
	if (g_fAuxHave) {
		for (i = 0; i < g_nAux; i++) {
			if (g_aAux[i][0] == uType) {
				errno = 0;
				return g_aAux[i][1];
			}
		}
		/*
		 * Walked a real auxv and type is absent — only soft-fill a
		 * few always-present keys so ENOENT still means "not there"
		 * for optional tags (AT_RANDOM, AT_EXECFN, …).
		 */
		if (uType == (unsigned long)AT_PAGESZ ||
		    uType == (unsigned long)AT_CLKTCK) {
			if (auxv_soft_default(uType, &uSoft)) {
				errno = 0;
				return uSoft;
			}
		}
		errno = ENOENT;
		return 0;
	}

	/* No stack auxv: freestanding soft defaults for common keys. */
	if (auxv_soft_default(uType, &uSoft)) {
		errno = 0;
		return uSoft;
	}
	errno = ENOENT;
	return 0;
}
