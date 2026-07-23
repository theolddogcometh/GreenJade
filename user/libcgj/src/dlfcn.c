/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room dlfcn bring-up stubs. Product dynamic loading is ld-gj;
 * export symbols so apps link against libc.so.6 without GNU libdl.
 */
#include <dlfcn.h>
#include <stddef.h>

static const char *s_szDlErr;
static int s_fDlErrPending;

void *
dlopen(const char *szFile, int nMode)
{
    (void)szFile;
    (void)nMode;
    s_szDlErr = "dlopen: not available (libcgj stub; use ld-gj)";
    s_fDlErrPending = 1;
    return NULL;
}

void *
dlsym(void *pHandle, const char *szSymbol)
{
    (void)pHandle;
    (void)szSymbol;
    s_szDlErr = "dlsym: not available (libcgj stub; use ld-gj)";
    s_fDlErrPending = 1;
    return NULL;
}

int
dlclose(void *pHandle)
{
    if (pHandle == NULL) {
        s_szDlErr = "dlclose: invalid handle";
        s_fDlErrPending = 1;
        return -1;
    }
    s_szDlErr = "dlclose: not available (libcgj stub)";
    s_fDlErrPending = 1;
    return -1;
}

char *
dlerror(void)
{
    const char *sz;

    if (!s_fDlErrPending) {
        return NULL;
    }
    s_fDlErrPending = 0;
    sz = s_szDlErr;
    s_szDlErr = NULL;
    return (char *)sz;
}

int
dladdr(const void *pAddr, void *pInfo)
{
    (void)pAddr;
    (void)pInfo;
    s_szDlErr = "dladdr: not available (libcgj stub)";
    s_fDlErrPending = 1;
    return 0;
}

void *
dlvsym(void *pHandle, const char *szSymbol, const char *szVersion)
{
    (void)pHandle;
    (void)szSymbol;
    (void)szVersion;
    s_szDlErr = "dlvsym: not available (libcgj stub; use ld-gj)";
    s_fDlErrPending = 1;
    return NULL;
}
