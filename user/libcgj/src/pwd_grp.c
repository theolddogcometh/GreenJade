/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * passwd/group soft fill: synthetic root/sshd/jay + optional /etc/passwd
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
static char g_szSshd[] = "sshd";
static char g_szJay[] = "jay";
static char g_szTty[] = "tty";
static char g_szNobody[] = "nobody";
static char g_szX[] = "x";
static char g_szHome[] = "/";
static char g_szJayHome[] = "/home/jay";
static char g_szSshdHome[] = "/var/empty";
static char g_szNobodyHome[] = "/nonexistent";
static char g_szShell[] = "/bin/sh";
static char g_szNologin[] = "/usr/sbin/nologin";
static char *g_aEmptyMem[] = { NULL };
static char *g_aJayMem[] = { g_szJay, NULL };

struct pw_syn {
    char *szName;
    uid_t uid;
    gid_t gid;
    char *szGecos;
    char *szDir;
    char *szShell;
};

struct gr_syn {
    char  *szName;
    gid_t  gid;
    char **ppMem;
};

/* Bring-up table only: root + sshd privsep + lab user jay. Not NSS. */
static const struct pw_syn g_aPwSyn[] = {
    { g_szRoot, 0, 0, g_szRoot, g_szHome, g_szShell },
    { g_szSshd, 74, 74, g_szSshd, g_szSshdHome, g_szNologin },
    { g_szJay, 1000, 1000, g_szJay, g_szJayHome, g_szShell },
    { g_szNobody, 65534, 65534, g_szNobody, g_szNobodyHome, g_szNologin }
};

static const struct gr_syn g_aGrSyn[] = {
    { g_szRoot, 0, g_aEmptyMem },
    { g_szTty, 5, g_aEmptyMem }, /* OpenSSH pty_setowner */
    { g_szSshd, 74, g_aEmptyMem },
    { g_szJay, 1000, g_aJayMem },
    { g_szNobody, 65534, g_aEmptyMem }
};

static struct passwd g_pw = {
    g_szRoot, g_szX, 0, 0, g_szRoot, g_szHome, g_szShell
};
static struct group g_gr = { g_szRoot, g_szX, 0, g_aEmptyMem };

/* File-scan cursor state for getpwent / getgrent soft path. */
static FILE *g_pPwFile;
static FILE *g_pGrFile;
static int g_fPwSynthetic;
static int g_fGrSynthetic;
static int g_iPwSyn;
static int g_iGrSyn;
static int g_fPwDone;
static int g_fGrDone;

/* Scratch for non-_r lookups that parse a line. */
static char g_aPwLine[512];
static char g_aGrLine[512];
static char *g_aGrMem[32];
static char g_aGrMemNames[256];
static char g_aSetLogin[32];
static int g_fSetLogin;

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
    /* Fail closed: empty passwd field is ambient auth. Not a user. */
    if (szPass[0] == '\0') {
        return -1;
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
passwd_from_syn(const struct pw_syn *pSyn)
{
    g_pw.pw_name = pSyn->szName;
    g_pw.pw_passwd = g_szX;
    g_pw.pw_uid = pSyn->uid;
    g_pw.pw_gid = pSyn->gid;
    g_pw.pw_gecos = pSyn->szGecos;
    g_pw.pw_dir = pSyn->szDir;
    g_pw.pw_shell = pSyn->szShell;
    return &g_pw;
}

static struct group *
group_from_syn(const struct gr_syn *pSyn)
{
    g_gr.gr_name = pSyn->szName;
    g_gr.gr_passwd = g_szX;
    g_gr.gr_gid = pSyn->gid;
    g_gr.gr_mem = pSyn->ppMem;
    return &g_gr;
}

static const struct pw_syn *
pw_syn_by_uid(uid_t uid)
{
    size_t iSyn;

    for (iSyn = 0; iSyn < sizeof(g_aPwSyn) / sizeof(g_aPwSyn[0]); iSyn++) {
        if (g_aPwSyn[iSyn].uid == uid) {
            return &g_aPwSyn[iSyn];
        }
    }
    return NULL;
}

static const struct pw_syn *
pw_syn_by_name(const char *szName)
{
    size_t iSyn;

    if (szName == NULL) {
        return NULL;
    }
    for (iSyn = 0; iSyn < sizeof(g_aPwSyn) / sizeof(g_aPwSyn[0]); iSyn++) {
        if (strcmp(g_aPwSyn[iSyn].szName, szName) == 0) {
            return &g_aPwSyn[iSyn];
        }
    }
    return NULL;
}

static const struct gr_syn *
gr_syn_by_gid(gid_t gid)
{
    size_t iSyn;

    for (iSyn = 0; iSyn < sizeof(g_aGrSyn) / sizeof(g_aGrSyn[0]); iSyn++) {
        if (g_aGrSyn[iSyn].gid == gid) {
            return &g_aGrSyn[iSyn];
        }
    }
    return NULL;
}

static const struct gr_syn *
gr_syn_by_name(const char *szName)
{
    size_t iSyn;

    if (szName == NULL) {
        return NULL;
    }
    for (iSyn = 0; iSyn < sizeof(g_aGrSyn) / sizeof(g_aGrSyn[0]); iSyn++) {
        if (strcmp(g_aGrSyn[iSyn].szName, szName) == 0) {
            return &g_aGrSyn[iSyn];
        }
    }
    return NULL;
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
    {
        const struct pw_syn *pSyn = pw_syn_by_uid(uid);

        if (pSyn != NULL) {
            return passwd_from_syn(pSyn);
        }
    }
    errno = 0;
    return NULL;
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
    {
        const struct pw_syn *pSyn = pw_syn_by_name(szName);

        if (pSyn != NULL) {
            return passwd_from_syn(pSyn);
        }
    }
    errno = 0;
    return NULL;
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
            g_iPwSyn = 0;
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
    if (g_fPwSynthetic) {
        if (g_iPwSyn < (int)(sizeof(g_aPwSyn) / sizeof(g_aPwSyn[0]))) {
            return passwd_from_syn(&g_aPwSyn[g_iPwSyn++]);
        }
        g_fPwDone = 1;
        return NULL;
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
    g_iPwSyn = 0;
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
    {
        const struct gr_syn *pSyn = gr_syn_by_gid(gid);

        if (pSyn != NULL) {
            return group_from_syn(pSyn);
        }
    }
    errno = 0;
    return NULL;
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
    {
        const struct gr_syn *pSyn = gr_syn_by_name(szName);

        if (pSyn != NULL) {
            return group_from_syn(pSyn);
        }
    }
    errno = 0;
    return NULL;
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
            g_iGrSyn = 0;
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
    if (g_fGrSynthetic) {
        if (g_iGrSyn < (int)(sizeof(g_aGrSyn) / sizeof(g_aGrSyn[0]))) {
            return group_from_syn(&g_aGrSyn[g_iGrSyn++]);
        }
        g_fGrDone = 1;
        return NULL;
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
    g_iGrSyn = 0;
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

int
setlogin(const char *szName)
{
    size_t n;

    if (szName == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = strlen(szName);
    if (n >= sizeof(g_aSetLogin)) {
        n = sizeof(g_aSetLogin) - 1;
    }
    memcpy(g_aSetLogin, szName, n);
    g_aSetLogin[n] = '\0';
    g_fSetLogin = 1;
    return 0;
}

char *
getlogin(void)
{
    char *p;

    if (g_fSetLogin && g_aSetLogin[0] != '\0') {
        return g_aSetLogin;
    }
    p = getenv("LOGNAME");
    if (p == NULL) {
        p = getenv("USER");
    }
    if (p == NULL || p[0] == '\0') {
        return g_szJay;
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

int
setpassent(int nStayopen)
{
    (void)nStayopen;
    setpwent();
    return 1;
}

int
setgroupent(int nStayopen)
{
    (void)nStayopen;
    setgrent();
    return 1;
}

#define GJ_IDCACHE 8

struct uid_cache {
    int   fUsed;
    uid_t uid;
    char  szName[32];
};

struct gid_cache {
    int   fUsed;
    gid_t gid;
    char  szName[32];
};

static struct uid_cache g_aUidCache[GJ_IDCACHE];
static struct gid_cache g_aGidCache[GJ_IDCACHE];
static unsigned g_uUidClock;
static unsigned g_uGidClock;

static void
fmt_id(char *sz, size_t cb, unsigned long u)
{
    char aDig[16];
    int n = 0;
    size_t o = 0;

    if (cb == 0) {
        return;
    }
    if (u == 0UL) {
        aDig[n++] = '0';
    } else {
        while (u > 0UL && n < 16) {
            aDig[n++] = (char)('0' + (int)(u % 10UL));
            u /= 10UL;
        }
    }
    while (n > 0 && o + 1 < cb) {
        sz[o++] = aDig[--n];
    }
    sz[o] = '\0';
}

char *
user_from_uid(uid_t uid, int nNouser)
{
    struct passwd *pPw;
    unsigned i;
    unsigned iSlot;

    for (i = 0; i < GJ_IDCACHE; i++) {
        if (g_aUidCache[i].fUsed && g_aUidCache[i].uid == uid) {
            return g_aUidCache[i].szName;
        }
    }
    pPw = getpwuid(uid);
    if (pPw == NULL || pPw->pw_name == NULL) {
        if (nNouser) {
            return NULL;
        }
    }
    iSlot = g_uUidClock % GJ_IDCACHE;
    g_uUidClock++;
    g_aUidCache[iSlot].fUsed = 1;
    g_aUidCache[iSlot].uid = uid;
    if (pPw != NULL && pPw->pw_name != NULL) {
        size_t n = strlen(pPw->pw_name);

        if (n >= sizeof(g_aUidCache[iSlot].szName)) {
            n = sizeof(g_aUidCache[iSlot].szName) - 1;
        }
        memcpy(g_aUidCache[iSlot].szName, pPw->pw_name, n);
        g_aUidCache[iSlot].szName[n] = '\0';
    } else {
        fmt_id(g_aUidCache[iSlot].szName, sizeof(g_aUidCache[iSlot].szName),
               (unsigned long)uid);
    }
    return g_aUidCache[iSlot].szName;
}

char *
group_from_gid(gid_t gid, int nNogroup)
{
    struct group *pGr;
    unsigned i;
    unsigned iSlot;

    for (i = 0; i < GJ_IDCACHE; i++) {
        if (g_aGidCache[i].fUsed && g_aGidCache[i].gid == gid) {
            return g_aGidCache[i].szName;
        }
    }
    pGr = getgrgid(gid);
    if (pGr == NULL || pGr->gr_name == NULL) {
        if (nNogroup) {
            return NULL;
        }
    }
    iSlot = g_uGidClock % GJ_IDCACHE;
    g_uGidClock++;
    g_aGidCache[iSlot].fUsed = 1;
    g_aGidCache[iSlot].gid = gid;
    if (pGr != NULL && pGr->gr_name != NULL) {
        size_t n = strlen(pGr->gr_name);

        if (n >= sizeof(g_aGidCache[iSlot].szName)) {
            n = sizeof(g_aGidCache[iSlot].szName) - 1;
        }
        memcpy(g_aGidCache[iSlot].szName, pGr->gr_name, n);
        g_aGidCache[iSlot].szName[n] = '\0';
    } else {
        fmt_id(g_aGidCache[iSlot].szName, sizeof(g_aGidCache[iSlot].szName),
               (unsigned long)gid);
    }
    return g_aGidCache[iSlot].szName;
}

int
uid_from_user(const char *szName, uid_t *pUid)
{
    struct passwd *pPw;

    if (szName == NULL || pUid == NULL) {
        errno = EINVAL;
        return -1;
    }
    pPw = getpwnam(szName);
    if (pPw == NULL) {
        return -1;
    }
    *pUid = pPw->pw_uid;
    return 0;
}

int
gid_from_group(const char *szName, gid_t *pGid)
{
    struct group *pGr;

    if (szName == NULL || pGid == NULL) {
        errno = EINVAL;
        return -1;
    }
    pGr = getgrnam(szName);
    if (pGr == NULL) {
        return -1;
    }
    *pGid = pGr->gr_gid;
    return 0;
}

struct passwd *
pw_dup(const struct passwd *pSrc)
{
    size_t cbName;
    size_t cbPass;
    size_t cbGecos;
    size_t cbDir;
    size_t cbShell;
    size_t cb;
    char *pBlk;
    struct passwd *pDst;
    char *pOff;

    if (pSrc == NULL) {
        errno = EINVAL;
        return NULL;
    }
    cbName = (pSrc->pw_name != NULL) ? strlen(pSrc->pw_name) + 1 : 1;
    cbPass = (pSrc->pw_passwd != NULL) ? strlen(pSrc->pw_passwd) + 1 : 1;
    cbGecos = (pSrc->pw_gecos != NULL) ? strlen(pSrc->pw_gecos) + 1 : 1;
    cbDir = (pSrc->pw_dir != NULL) ? strlen(pSrc->pw_dir) + 1 : 1;
    cbShell = (pSrc->pw_shell != NULL) ? strlen(pSrc->pw_shell) + 1 : 1;
    cb = sizeof(*pDst) + cbName + cbPass + cbGecos + cbDir + cbShell;
    pBlk = malloc(cb);
    if (pBlk == NULL) {
        return NULL;
    }
    pDst = (struct passwd *)(void *)pBlk;
    pOff = pBlk + sizeof(*pDst);
    pDst->pw_uid = pSrc->pw_uid;
    pDst->pw_gid = pSrc->pw_gid;
    pDst->pw_name = pOff;
    if (pSrc->pw_name != NULL) {
        memcpy(pOff, pSrc->pw_name, cbName);
    } else {
        pOff[0] = '\0';
    }
    pOff += cbName;
    pDst->pw_passwd = pOff;
    if (pSrc->pw_passwd != NULL) {
        memcpy(pOff, pSrc->pw_passwd, cbPass);
    } else {
        pOff[0] = '\0';
    }
    pOff += cbPass;
    pDst->pw_gecos = pOff;
    if (pSrc->pw_gecos != NULL) {
        memcpy(pOff, pSrc->pw_gecos, cbGecos);
    } else {
        pOff[0] = '\0';
    }
    pOff += cbGecos;
    pDst->pw_dir = pOff;
    if (pSrc->pw_dir != NULL) {
        memcpy(pOff, pSrc->pw_dir, cbDir);
    } else {
        pOff[0] = '\0';
    }
    pOff += cbDir;
    pDst->pw_shell = pOff;
    if (pSrc->pw_shell != NULL) {
        memcpy(pOff, pSrc->pw_shell, cbShell);
    } else {
        pOff[0] = '\0';
    }
    return pDst;
}
