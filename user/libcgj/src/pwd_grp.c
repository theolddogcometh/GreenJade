/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * passwd/group bring-up stubs (root + single primary group).
 */
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char g_szRoot[] = "root";
static char g_szX[] = "x";
static char g_szHome[] = "/";
static char g_szShell[] = "/bin/sh";
static char *g_aEmptyMem[] = { NULL };

static struct passwd g_pw = {
    g_szRoot, g_szX, 0, 0, g_szRoot, g_szHome, g_szShell
};
static struct group g_gr = { g_szRoot, g_szX, 0, g_aEmptyMem };
static int g_fPw = 0;
static int g_fGr = 0;

struct passwd *
getpwuid(uid_t uid)
{
    if (uid != 0 && uid != getuid()) {
        errno = 0;
        return NULL;
    }
    g_pw.pw_uid = uid;
    g_pw.pw_gid = getgid();
    return &g_pw;
}

struct passwd *
getpwnam(const char *szName)
{
    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (strcmp(szName, "root") != 0) {
        errno = 0;
        return NULL;
    }
    g_pw.pw_uid = 0;
    g_pw.pw_gid = 0;
    return &g_pw;
}

struct passwd *
getpwent(void)
{
    if (g_fPw) {
        return NULL;
    }
    g_fPw = 1;
    return getpwuid(0);
}

void
setpwent(void)
{
    g_fPw = 0;
}

void
endpwent(void)
{
    g_fPw = 0;
}

int
getpwuid_r(uid_t uid, struct passwd *pPwd, char *szBuf, size_t cb,
           struct passwd **ppResult)
{
    struct passwd *p;

    (void)szBuf;
    (void)cb;
    if (pPwd == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = getpwuid(uid);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pPwd = *p;
    *ppResult = pPwd;
    return 0;
}

int
getpwnam_r(const char *szName, struct passwd *pPwd, char *szBuf, size_t cb,
           struct passwd **ppResult)
{
    struct passwd *p;

    (void)szBuf;
    (void)cb;
    if (pPwd == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = getpwnam(szName);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pPwd = *p;
    *ppResult = pPwd;
    return 0;
}

struct group *
getgrgid(gid_t gid)
{
    if (gid != 0 && gid != getgid()) {
        errno = 0;
        return NULL;
    }
    g_gr.gr_gid = gid;
    return &g_gr;
}

struct group *
getgrnam(const char *szName)
{
    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (strcmp(szName, "root") != 0) {
        errno = 0;
        return NULL;
    }
    g_gr.gr_gid = 0;
    return &g_gr;
}

struct group *
getgrent(void)
{
    if (g_fGr) {
        return NULL;
    }
    g_fGr = 1;
    return getgrgid(0);
}

void
setgrent(void)
{
    g_fGr = 0;
}

void
endgrent(void)
{
    g_fGr = 0;
}

int
getgrgid_r(gid_t gid, struct group *pGrp, char *szBuf, size_t cb,
           struct group **ppResult)
{
    struct group *p;

    (void)szBuf;
    (void)cb;
    if (pGrp == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = getgrgid(gid);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pGrp = *p;
    *ppResult = pGrp;
    return 0;
}

int
getgrnam_r(const char *szName, struct group *pGrp, char *szBuf, size_t cb,
           struct group **ppResult)
{
    struct group *p;

    (void)szBuf;
    (void)cb;
    if (pGrp == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = getgrnam(szName);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pGrp = *p;
    *ppResult = pGrp;
    return 0;
}

char *
getlogin(void)
{
    char *p = getenv("LOGNAME");

    if (p == NULL) {
        p = getenv("USER");
    }
    if (p == NULL) {
        return (char *)"user";
    }
    return p;
}

int
getlogin_r(char *szBuf, size_t cb)
{
    const char *p;
    size_t n;

    if (szBuf == NULL || cb == 0) {
        return EINVAL;
    }
    p = getlogin();
    n = strlen(p);
    if (n + 1 > cb) {
        return ERANGE;
    }
    memcpy(szBuf, p, n + 1);
    return 0;
}
