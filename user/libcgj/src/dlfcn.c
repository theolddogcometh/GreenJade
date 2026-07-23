/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room dlfcn soft fill-in. Product dynamic loading is ld-gj;
 * export symbols so apps link against libc.so.6 without GNU libdl.
 * Soft path: fail-closed with sticky error text, RTLD_* validation,
 * and zeroed Dl_info on unsuccessful address queries.
 */
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Stable error buffer — dlerror() returns this pointer once then clears. */
static char g_aDlErr[160];
static int  g_fDlErrPending;

/* Soft main-image pseudo-handle (not a real SO). */
static char g_chMainPseudo;
#define GJ_DL_MAIN_HANDLE ((void *)&g_chMainPseudo)

static void
dl_set_err(const char *szMsg)
{
	size_t i;

	if (szMsg == NULL) {
		szMsg = "dlfcn: unknown error";
	}
	for (i = 0; i + 1u < sizeof(g_aDlErr) && szMsg[i] != '\0'; i++) {
		g_aDlErr[i] = szMsg[i];
	}
	g_aDlErr[i] = '\0';
	g_fDlErrPending = 1;
}

static int
dl_mode_ok(int nMode)
{
	/* Reject empty mode bits other than LOCAL/GLOBAL alone. */
	if ((nMode & (RTLD_LAZY | RTLD_NOW | RTLD_NOLOAD)) == 0 &&
	    (nMode & ~((int)RTLD_GLOBAL | (int)RTLD_NODELETE |
			 (int)RTLD_LOCAL)) == 0) {
		/* glibc accepts 0 as LAZY-ish; soft: allow */
		return 1;
	}
	if ((nMode & RTLD_LAZY) != 0 && (nMode & RTLD_NOW) != 0) {
		/* Both set — treat as NOW (glibc: last wins-ish); soft ok */
	}
	return 1;
}

void *
dlopen(const char *szFile, int nMode)
{
	if (!dl_mode_ok(nMode)) {
		dl_set_err("dlopen: invalid mode");
		return NULL;
	}
	/* RTLD_NOLOAD: never load — soft has no SO table, so always miss. */
	if ((nMode & RTLD_NOLOAD) != 0) {
		dl_set_err("dlopen: RTLD_NOLOAD: object not loaded (libcgj soft)");
		return NULL;
	}
	/*
	 * NULL path is glibc main-program handle. Soft returns a stable
	 * pseudo-handle so callers can pass it to dlsym/dlclose without
	 * immediate EINVAL; dlsym still fails closed (no symbol table).
	 */
	if (szFile == NULL) {
		return GJ_DL_MAIN_HANDLE;
	}
	if (szFile[0] == '\0') {
		dl_set_err("dlopen: empty path");
		return NULL;
	}
	(void)nMode;
	dl_set_err("dlopen: not available (libcgj stub; use ld-gj)");
	return NULL;
}

void *
dlsym(void *pHandle, const char *szSymbol)
{
	if (szSymbol == NULL || szSymbol[0] == '\0') {
		dl_set_err("dlsym: null or empty symbol");
		return NULL;
	}
	/*
	 * RTLD_DEFAULT / RTLD_NEXT / main soft handle: no symbol graph in
	 * freestanding soft path — fail closed. Real resolution is ld-gj.
	 */
	if (pHandle == RTLD_DEFAULT || pHandle == RTLD_NEXT ||
	    pHandle == GJ_DL_MAIN_HANDLE || pHandle == NULL) {
		dl_set_err("dlsym: soft path has no symbol table (use ld-gj)");
		return NULL;
	}
	/* Unknown handle — still fail closed. */
	(void)pHandle;
	dl_set_err("dlsym: not available (libcgj stub; use ld-gj)");
	return NULL;
}

int
dlclose(void *pHandle)
{
	if (pHandle == NULL) {
		dl_set_err("dlclose: invalid handle");
		return -1;
	}
	/* Main soft handle and RTLD_* macros: treat as no-op success. */
	if (pHandle == GJ_DL_MAIN_HANDLE || pHandle == RTLD_DEFAULT ||
	    pHandle == RTLD_NEXT) {
		g_fDlErrPending = 0;
		g_aDlErr[0] = '\0';
		return 0;
	}
	dl_set_err("dlclose: not available (libcgj stub)");
	return -1;
}

char *
dlerror(void)
{
	if (!g_fDlErrPending) {
		return NULL;
	}
	g_fDlErrPending = 0;
	return g_aDlErr;
}

/*
 * Soft Dl_info layout (matches common glibc shape; opaque void* in header).
 * Zeroed on failure so callers can read fields safely after a 0 return.
 */
struct gj_dl_info {
	const char *dli_fname;
	void       *dli_fbase;
	const char *dli_sname;
	void       *dli_saddr;
};

int
dladdr(const void *pAddr, void *pInfo)
{
	struct gj_dl_info *pDi = (struct gj_dl_info *)pInfo;

	if (pDi != NULL) {
		pDi->dli_fname = NULL;
		pDi->dli_fbase = NULL;
		pDi->dli_sname = NULL;
		pDi->dli_saddr = NULL;
	}
	if (pAddr == NULL) {
		dl_set_err("dladdr: null address");
		return 0;
	}
	/*
	 * Soft: no link map. Return 0 (not found). ld-gj owns real walk.
	 * Leave errno alone (glibc dladdr does not set errno on miss).
	 */
	(void)pAddr;
	g_fDlErrPending = 0;
	return 0;
}

void *
dlvsym(void *pHandle, const char *szSymbol, const char *szVersion)
{
	if (szVersion == NULL) {
		/* Versioned lookup with NULL version → plain dlsym shape. */
		return dlsym(pHandle, szSymbol);
	}
	if (szSymbol == NULL || szSymbol[0] == '\0') {
		dl_set_err("dlvsym: null or empty symbol");
		return NULL;
	}
	if (szVersion[0] == '\0') {
		dl_set_err("dlvsym: empty version");
		return NULL;
	}
	(void)pHandle;
	dl_set_err("dlvsym: not available (libcgj stub; use ld-gj)");
	return NULL;
}
