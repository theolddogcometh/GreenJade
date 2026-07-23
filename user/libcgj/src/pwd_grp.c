/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * passwd/group soft fill: synthetic root + optional /etc/passwd|/etc/group
 * scan. fgetpwent/putpwent/getpwent_r live in graph batches — keep those out.
 */
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
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

/* File-scan cursor state for getpwent / getgrent soft path. */
static FILE *g_pPwFile;
static FILE *g_pGrFile;
static int g_fPwSynthetic;
static int g_fGrSynthetic;
static int g_fPwDone;
static int g_fGrDone;

/* Scratch for non-_r lookups that parse a line. */
static char g_aPwLine[512];
static char g_aGrLine[512];
static char *g_aGrMem[32];
static char g_aGrMemNames[256];

static int
copy_field(char **ppDst, char *szBuf, size_t *pOff, size_t cb, const char *szSrc)
{
    size_t n;
    size_t off;

    if (szSrc == NULL) {
        szSrc = "";
    }
    n = strlen(szSrc);
    off = *pOff;
    if (off + n + 1 > cb) {
        return ERANGE;
    }
    memcpy(szBuf + off, szSrc, n + 1);
    *ppDst = szBuf + off;
    *pOff = off + n + 1;
    return 0;
}

static int
pack_passwd(struct passwd *pDst, char *szBuf, size_t cb,
            const struct passwd *pSrc)
{
    size_t off = 0;
    int nErr;

    if (pDst == NULL || pSrc == NULL || szBuf == NULL) {
        return EINVAL;
    }
    nErr = copy_field(&pDst->pw_name, szBuf, &off, cb, pSrc->pw_name);
    if (nErr != 0) {
        return nErr;
    }
    nErr = copy_field(&pDst->pw_passwd, szBuf, &off, cb, pSrc->pw_passwd);
    if (nErr != 0) {
        return nErr;
    }
    nErr = copy_field(&pDst->pw_gecos, szBuf, &off, cb, pSrc->pw_gecos);
    if (nErr != 0) {
        return nErr;
    }
    nErr = copy_field(&pDst->pw_dir, szBuf, &off, cb, pSrc->pw_dir);
    if (nErr != 0) {
        return nErr;
    }
    nErr = copy_field(&pDst->pw_shell, szBuf, &off, cb, pSrc->pw_shell);
    if (nErr != 0) {
        return nErr;
    }
    pDst->pw_uid = pSrc->pw_uid;
    pDst->pw_gid = pSrc->pw_gid;
    return 0;
}

static int
pack_group(struct group *pDst, char *szBuf, size_t cb, const struct group *pSrc)
{
    size_t off = 0;
    size_t i;
    size_t cMem = 0;
    char **ppMem;
    int nErr;

    if (pDst == NULL || pSrc == NULL || szBuf == NULL) {
        return EINVAL;
    }
    nErr = copy_field(&pDst->gr_name, szBuf, &off, cb, pSrc->gr_name);
    if (nErr != 0) {
        return nErr;
    }
    nErr = copy_field(&pDst->gr_passwd, szBuf, &off, cb, pSrc->gr_passwd);
    if (nErr != 0) {
        return nErr;
    }
    pDst->gr_gid = pSrc->gr_gid;
    if (pSrc->gr_mem != NULL) {
        while (pSrc->gr_mem[cMem] != NULL) {
            cMem++;
        }
    }
    /* Align pointer array. */
    while (off % sizeof(char *) != 0) {
        if (off + 1 > cb) {
            return ERANGE;
        }
        szBuf[off++] = '\0';
    }
    if (off + (cMem + 1) * sizeof(char *) > cb) {
        return ERANGE;
    }
    ppMem = (char **)(void *)(szBuf + off);
    off += (cMem + 1) * sizeof(char *);
    for (i = 0; i < cMem; i++) {
        nErr = copy_field(&ppMem[i], szBuf, &off, cb, pSrc->gr_mem[i]);
        if (nErr != 0) {
            return nErr;
        }
    }
    ppMem[cMem] = NULL;
    pDst->gr_mem = ppMem;
    return 0;
}

static int
parse_pw_line(char *szLine, struct passwd *pPw)
{
    char *szName;
    char *szPass;
    char *szUid;
    char *szGid;
    char *szGecos;
    char *szDir;
    char *szShell;
    char *p;

    if (szLine == NULL || pPw == NULL) {
        return -1;
    }
    p = szLine;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\0' || *p == '\n' || *p == '#') {
        return -1;
    }
    szName = p;
    szPass = strchr(p, ':');
    if (szPass == NULL) {
        return -1;
    }
    *szPass++ = '\0';
    szUid = strchr(szPass, ':');
    if (szUid == NULL) {
        return -1;
    }
    *szUid++ = '\0';
    szGid = strchr(szUid, ':');
    if (szGid == NULL) {
        return -1;
    }
    *szGid++ = '\0';
    szGecos = strchr(szGid, ':');
    if (szGecos == NULL) {
        return -1;
    }
    *szGecos++ = '\0';
    szDir = strchr(szGecos, ':');
    if (szDir == NULL) {
        return -1;
    }
    *szDir++ = '\0';
    szShell = strchr(szDir, ':');
    if (szShell == NULL) {
        return -1;
    }
    *szShell++ = '\0';
    {
        char *nl = strchr(szShell, '\n');

        if (nl != NULL) {
            *nl = '\0';
        }
    }
    pPw->pw_name = szName;
    pPw->pw_passwd = szPass;
    pPw->pw_uid = (uid_t)strtoul(szUid, NULL, 10);
    pPw->pw_gid = (gid_t)strtoul(szGid, NULL, 10);
    pPw->pw_gecos = szGecos;
    pPw->pw_dir = szDir;
    pPw->pw_shell = szShell;
    return 0;
}

static int
parse_gr_line(char *szLine, struct group *pGr)
{
    char *szName;
    char *szPass;
    char *szGid;
    char *szMem;
    char *p;
    size_t iMem = 0;
    size_t off = 0;

    if (szLine == NULL || pGr == NULL) {
        return -1;
    }
    p = szLine;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\0' || *p == '\n' || *p == '#') {
        return -1;
    }
    szName = p;
    szPass = strchr(p, ':');
    if (szPass == NULL) {
        return -1;
    }
    *szPass++ = '\0';
    szGid = strchr(szPass, ':');
    if (szGid == NULL) {
        return -1;
    }
    *szGid++ = '\0';
    szMem = strchr(szGid, ':');
    if (szMem != NULL) {
        *szMem++ = '\0';
    } else {
        szMem = (char *)"";
    }
    {
        char *nl = strchr(szGid, '\n');

        if (nl != NULL) {
            *nl = '\0';
        }
        nl = strchr(szMem, '\n');
        if (nl != NULL) {
            *nl = '\0';
        }
    }
    pGr->gr_name = szName;
    pGr->gr_passwd = szPass;
    pGr->gr_gid = (gid_t)strtoul(szGid, NULL, 10);
    memset(g_aGrMem, 0, sizeof(g_aGrMem));
    memset(g_aGrMemNames, 0, sizeof(g_aGrMemNames));
    while (*szMem != '\0' && iMem + 1 < sizeof(g_aGrMem) / sizeof(g_aGrMem[0])) {
        char *szTok = szMem;
        char *szComma = strchr(szMem, ',');
        size_t n;

        if (szComma != NULL) {
            *szComma = '\0';
            szMem = szComma + 1;
        } else {
            szMem = szMem + strlen(szMem);
        }
        n = strlen(szTok);
        if (n == 0) {
            continue;
        }
        if (off + n + 1 >= sizeof(g_aGrMemNames)) {
            break;
        }
        memcpy(g_aGrMemNames + off, szTok, n + 1);
        g_aGrMem[iMem++] = g_aGrMemNames + off;
        off += n + 1;
    }
    g_aGrMem[iMem] = NULL;
    pGr->gr_mem = g_aGrMem;
    return 0;
}

static struct passwd *
synthetic_pw(uid_t uid)
{
    g_pw.pw_name = g_szRoot;
    g_pw.pw_passwd = g_szX;
    g_pw.pw_uid = uid;
    g_pw.pw_gid = (uid == 0) ? 0 : getgid();
    g_pw.pw_gecos = g_szRoot;
    g_pw.pw_dir = g_szHome;
    g_pw.pw_shell = g_szShell;
    return &g_pw;
}

static struct group *
synthetic_gr(gid_t gid)
{
    g_gr.gr_name = g_szRoot;
    g_gr.gr_passwd = g_szX;
    g_gr.gr_gid = gid;
    g_gr.gr_mem = g_aEmptyMem;
    return &g_gr;
}

struct passwd *
getpwuid(uid_t uid)
{
    FILE *pF;
    struct passwd pw;

    pF = fopen("/etc/passwd", "r");
    if (pF != NULL) {
        while (fgets(g_aPwLine, (int)sizeof(g_aPwLine), pF) != NULL) {
            if (parse_pw_line(g_aPwLine, &pw) == 0 && pw.pw_uid == uid) {
                g_pw = pw;
                (void)fclose(pF);
                return &g_pw;
            }
        }
        (void)fclose(pF);
    }
    if (uid != 0 && uid != getuid()) {
        errno = 0;
        return NULL;
    }
    return synthetic_pw(uid);
}

struct passwd *
getpwnam(const char *szName)
{
    FILE *pF;
    struct passwd pw;

    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    pF = fopen("/etc/passwd", "r");
    if (pF != NULL) {
        while (fgets(g_aPwLine, (int)sizeof(g_aPwLine), pF) != NULL) {
            if (parse_pw_line(g_aPwLine, &pw) == 0 &&
                strcmp(pw.pw_name, szName) == 0) {
                g_pw = pw;
                (void)fclose(pF);
                return &g_pw;
            }
        }
        (void)fclose(pF);
    }
    if (strcmp(szName, "root") != 0) {
        errno = 0;
        return NULL;
    }
    return synthetic_pw(0);
}

struct passwd *
getpwent(void)
{
    struct passwd pw;

    if (g_fPwDone) {
        return NULL;
    }
    if (g_pPwFile == NULL && !g_fPwSynthetic) {
        g_pPwFile = fopen("/etc/passwd", "r");
        if (g_pPwFile == NULL) {
            g_fPwSynthetic = 1;
        }
    }
    if (g_pPwFile != NULL) {
        while (fgets(g_aPwLine, (int)sizeof(g_aPwLine), g_pPwFile) != NULL) {
            if (parse_pw_line(g_aPwLine, &pw) == 0) {
                g_pw = pw;
                return &g_pw;
            }
        }
        (void)fclose(g_pPwFile);
        g_pPwFile = NULL;
        g_fPwDone = 1;
        return NULL;
    }
    /* Synthetic single-entry stream. */
    if (g_fPwSynthetic == 1) {
        g_fPwSynthetic = 2;
        return synthetic_pw(0);
    }
    g_fPwDone = 1;
    return NULL;
}

void
setpwent(void)
{
    if (g_pPwFile != NULL) {
        (void)fclose(g_pPwFile);
        g_pPwFile = NULL;
    }
    g_fPwSynthetic = 0;
    g_fPwDone = 0;
}

void
endpwent(void)
{
    setpwent();
}

int
getpwuid_r(uid_t uid, struct passwd *pPwd, char *szBuf, size_t cb,
           struct passwd **ppResult)
{
    struct passwd *p;
    int nErr;

    if (pPwd == NULL || ppResult == NULL || szBuf == NULL) {
        return EINVAL;
    }
    p = getpwuid(uid);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    nErr = pack_passwd(pPwd, szBuf, cb, p);
    if (nErr != 0) {
        *ppResult = NULL;
        return nErr;
    }
    *ppResult = pPwd;
    return 0;
}

int
getpwnam_r(const char *szName, struct passwd *pPwd, char *szBuf, size_t cb,
           struct passwd **ppResult)
{
    struct passwd *p;
    int nErr;

    if (pPwd == NULL || ppResult == NULL || szBuf == NULL) {
        return EINVAL;
    }
    p = getpwnam(szName);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    nErr = pack_passwd(pPwd, szBuf, cb, p);
    if (nErr != 0) {
        *ppResult = NULL;
        return nErr;
    }
    *ppResult = pPwd;
    return 0;
}

struct group *
getgrgid(gid_t gid)
{
    FILE *pF;
    struct group gr;

    pF = fopen("/etc/group", "r");
    if (pF != NULL) {
        while (fgets(g_aGrLine, (int)sizeof(g_aGrLine), pF) != NULL) {
            if (parse_gr_line(g_aGrLine, &gr) == 0 && gr.gr_gid == gid) {
                g_gr = gr;
                (void)fclose(pF);
                return &g_gr;
            }
        }
        (void)fclose(pF);
    }
    if (gid != 0 && gid != getgid()) {
        errno = 0;
        return NULL;
    }
    return synthetic_gr(gid);
}

struct group *
getgrnam(const char *szName)
{
    FILE *pF;
    struct group gr;

    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    pF = fopen("/etc/group", "r");
    if (pF != NULL) {
        while (fgets(g_aGrLine, (int)sizeof(g_aGrLine), pF) != NULL) {
            if (parse_gr_line(g_aGrLine, &gr) == 0 &&
                strcmp(gr.gr_name, szName) == 0) {
                g_gr = gr;
                (void)fclose(pF);
                return &g_gr;
            }
        }
        (void)fclose(pF);
    }
    if (strcmp(szName, "root") != 0) {
        errno = 0;
        return NULL;
    }
    return synthetic_gr(0);
}

struct group *
getgrent(void)
{
    struct group gr;

    if (g_fGrDone) {
        return NULL;
    }
    if (g_pGrFile == NULL && !g_fGrSynthetic) {
        g_pGrFile = fopen("/etc/group", "r");
        if (g_pGrFile == NULL) {
            g_fGrSynthetic = 1;
        }
    }
    if (g_pGrFile != NULL) {
        while (fgets(g_aGrLine, (int)sizeof(g_aGrLine), g_pGrFile) != NULL) {
            if (parse_gr_line(g_aGrLine, &gr) == 0) {
                g_gr = gr;
                return &g_gr;
            }
        }
        (void)fclose(g_pGrFile);
        g_pGrFile = NULL;
        g_fGrDone = 1;
        return NULL;
    }
    if (g_fGrSynthetic == 1) {
        g_fGrSynthetic = 2;
        return synthetic_gr(0);
    }
    g_fGrDone = 1;
    return NULL;
}

void
setgrent(void)
{
    if (g_pGrFile != NULL) {
        (void)fclose(g_pGrFile);
        g_pGrFile = NULL;
    }
    g_fGrSynthetic = 0;
    g_fGrDone = 0;
}

void
endgrent(void)
{
    setgrent();
}

int
getgrgid_r(gid_t gid, struct group *pGrp, char *szBuf, size_t cb,
           struct group **ppResult)
{
    struct group *p;
    int nErr;

    if (pGrp == NULL || ppResult == NULL || szBuf == NULL) {
        return EINVAL;
    }
    p = getgrgid(gid);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    nErr = pack_group(pGrp, szBuf, cb, p);
    if (nErr != 0) {
        *ppResult = NULL;
        return nErr;
    }
    *ppResult = pGrp;
    return 0;
}

int
getgrnam_r(const char *szName, struct group *pGrp, char *szBuf, size_t cb,
           struct group **ppResult)
{
    struct group *p;
    int nErr;

    if (pGrp == NULL || ppResult == NULL || szBuf == NULL) {
        return EINVAL;
    }
    p = getgrnam(szName);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    nErr = pack_group(pGrp, szBuf, cb, p);
    if (nErr != 0) {
        *ppResult = NULL;
        return nErr;
    }
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
    if (p == NULL || p[0] == '\0') {
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
