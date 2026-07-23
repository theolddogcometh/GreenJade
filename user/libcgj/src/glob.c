/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft glob: multi-segment * / ? / [], MARK, PERIOD, TILDE, ONLYDIR,
 * errfunc. No brace expand. Clean-room pure C.
 */
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define GJ_GLOB_PATH_MAX 512

static int
path_join(char *szOut, size_t cb, const char *szDir, const char *szName)
{
    size_t nD = (szDir != NULL) ? strlen(szDir) : 0;
    size_t nN = (szName != NULL) ? strlen(szName) : 0;
    size_t i = 0;
    size_t j;
    int fNeedSlash;

    if (cb == 0 || szOut == NULL) {
        return -1;
    }
    if (nD + nN + 2 > cb) {
        return -1;
    }
    for (j = 0; j < nD; j++) {
        szOut[i++] = szDir[j];
    }
    fNeedSlash = (nD > 0 && szDir[nD - 1] != '/' && nN > 0);
    /* Root dir "/" already ends with slash. */
    if (fNeedSlash) {
        szOut[i++] = '/';
    }
    for (j = 0; j < nN; j++) {
        szOut[i++] = szName[j];
    }
    szOut[i] = '\0';
    return 0;
}

static int
glob_append(glob_t *pG, const char *szPath, int nFlags)
{
    char **ppNew;
    char *sz;
    size_t n = pG->gl_pathc;
    size_t nOff = (pG->gl_flags & GLOB_DOOFFS) ? pG->gl_offs : 0;
    size_t nSlots = nOff + n + 2;
    char aMark[GJ_GLOB_PATH_MAX + 2];
    const char *szAdd = szPath;
    struct stat st;

    if (szPath == NULL) {
        return GLOB_NOSPACE;
    }

    if ((nFlags & GLOB_ONLYDIR) != 0) {
        if (stat(szPath, &st) != 0 || !S_ISDIR(st.st_mode)) {
            return 0;
        }
    }

    if ((nFlags & GLOB_MARK) != 0) {
        if (stat(szPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            size_t nL = strlen(szPath);

            if (nL > 0 && szPath[nL - 1] != '/' && nL + 2 < sizeof(aMark)) {
                memcpy(aMark, szPath, nL);
                aMark[nL] = '/';
                aMark[nL + 1] = '\0';
                szAdd = aMark;
            }
        }
    }

    sz = strdup(szAdd);
    if (sz == NULL) {
        return GLOB_NOSPACE;
    }
    ppNew = (char **)realloc(pG->gl_pathv, nSlots * sizeof(char *));
    if (ppNew == NULL) {
        free(sz);
        return GLOB_NOSPACE;
    }
    pG->gl_pathv = ppNew;
    if (n == 0 && nOff > 0) {
        size_t k;

        for (k = 0; k < nOff; k++) {
            pG->gl_pathv[k] = NULL;
        }
    }
    pG->gl_pathv[nOff + n] = sz;
    pG->gl_pathv[nOff + n + 1] = NULL;
    pG->gl_pathc = n + 1;
    return 0;
}

static int
has_meta(const char *sz)
{
    int fEsc = 0;

    while (sz != NULL && *sz != '\0') {
        if (fEsc) {
            fEsc = 0;
            sz++;
            continue;
        }
        if (*sz == '\\') {
            fEsc = 1;
            sz++;
            continue;
        }
        if (*sz == '*' || *sz == '?' || *sz == '[') {
            return 1;
        }
        sz++;
    }
    return 0;
}

static int
call_err(int (*pfnErr)(const char *, int), const char *szPath, int nErr,
         int nFlags)
{
    if (pfnErr != NULL) {
        if (pfnErr(szPath, nErr) != 0) {
            return GLOB_ABORTED;
        }
    }
    if ((nFlags & GLOB_ERR) != 0) {
        return GLOB_ABORTED;
    }
    return 0;
}

/* Build next full path from base + name. Empty base → name; "/" + name → /name. */
static int
make_child(char *szOut, size_t cb, const char *szBase, const char *szName)
{
    if (szBase == NULL || szBase[0] == '\0') {
        size_t nL = strlen(szName);

        if (nL + 1 > cb) {
            return -1;
        }
        memcpy(szOut, szName, nL + 1);
        return 0;
    }
    return path_join(szOut, cb, szBase, szName);
}

/*
 * Expand remaining pattern szRest under directory szBase ("" = cwd-relative
 * root of the pattern; "/" = filesystem root).
 */
static int
glob_seg(const char *szBase, const char *szRest, int nFlags,
         int (*pfnErr)(const char *, int), glob_t *pGlob, int *pfAny)
{
    char aDir[GJ_GLOB_PATH_MAX];
    char aPat[256];
    char aNext[GJ_GLOB_PATH_MAX];
    const char *pSlash;
    const char *szOpen;
    DIR *pDir;
    struct dirent *pEnt;
    int nFnFlags = 0;
    int nRc;
    size_t nSeg;
    int fLast;

    if (szRest == NULL) {
        return 0;
    }

    /* Collapse leading '/' into absolute base. */
    if (szRest[0] == '/' && (szBase == NULL || szBase[0] == '\0')) {
        /* Pattern "//..." soft-normalize to single root. */
        while (szRest[0] == '/') {
            szRest++;
        }
        return glob_seg("/", szRest, nFlags, pfnErr, pGlob, pfAny);
    }

    if (szRest[0] == '\0') {
        if (szBase != NULL && szBase[0] != '\0' && access(szBase, F_OK) == 0) {
            nRc = glob_append(pGlob, szBase, nFlags);
            if (nRc == 0) {
                *pfAny = 1;
            }
            return nRc;
        }
        return 0;
    }

    pSlash = strchr(szRest, '/');
    if (pSlash != NULL) {
        nSeg = (size_t)(pSlash - szRest);
        fLast = 0;
    } else {
        nSeg = strlen(szRest);
        fLast = 1;
    }
    if (nSeg >= sizeof(aPat)) {
        nSeg = sizeof(aPat) - 1;
    }
    memcpy(aPat, szRest, nSeg);
    aPat[nSeg] = '\0';

    if (!has_meta(aPat)) {
        if (make_child(aNext, sizeof(aNext), szBase, aPat) != 0) {
            return GLOB_NOSPACE;
        }
        if (fLast) {
            if (access(aNext, F_OK) == 0) {
                nRc = glob_append(pGlob, aNext, nFlags);
                if (nRc == 0) {
                    *pfAny = 1;
                }
                return nRc;
            }
            return 0;
        }
        return glob_seg(aNext, pSlash + 1, nFlags, pfnErr, pGlob, pfAny);
    }

    /* Meta segment: scan directory. */
    if (szBase == NULL || szBase[0] == '\0') {
        aDir[0] = '.';
        aDir[1] = '\0';
        szOpen = aDir;
    } else {
        size_t nB = strlen(szBase);

        if (nB >= sizeof(aDir)) {
            return GLOB_NOSPACE;
        }
        memcpy(aDir, szBase, nB + 1);
        szOpen = aDir;
    }

    if ((nFlags & GLOB_NOESCAPE) != 0) {
        nFnFlags |= FNM_NOESCAPE;
    }
    if ((nFlags & GLOB_PERIOD) == 0) {
        nFnFlags |= FNM_PERIOD;
    }

    pDir = opendir(szOpen);
    if (pDir == NULL) {
        return call_err(pfnErr, szOpen, errno, nFlags);
    }

    while ((pEnt = readdir(pDir)) != NULL) {
        const char *szName = pEnt->d_name;

        if (szName[0] == '.' &&
            (szName[1] == '\0' ||
             (szName[1] == '.' && szName[2] == '\0'))) {
            if (aPat[0] != '.') {
                continue;
            }
        }
        if (fnmatch(aPat, szName, nFnFlags) != 0) {
            continue;
        }
        if (make_child(aNext, sizeof(aNext), szBase, szName) != 0) {
            continue;
        }
        if (fLast) {
            nRc = glob_append(pGlob, aNext, nFlags);
            if (nRc != 0) {
                closedir(pDir);
                return nRc;
            }
            *pfAny = 1;
        } else {
            nRc = glob_seg(aNext, pSlash + 1, nFlags, pfnErr, pGlob, pfAny);
            if (nRc != 0) {
                closedir(pDir);
                return nRc;
            }
        }
    }
    closedir(pDir);
    return 0;
}

static int
expand_tilde(const char *szPattern, char *szOut, size_t cb)
{
    const char *szHome;
    size_t nH;
    size_t nR;
    const char *szRest;

    if (szPattern == NULL || szPattern[0] != '~') {
        return -1;
    }
    /* Bare ~ or ~/... only (no ~user). */
    if (szPattern[1] != '\0' && szPattern[1] != '/') {
        return -1;
    }
    szHome = getenv("HOME");
    if (szHome == NULL || szHome[0] == '\0') {
        return -1;
    }
    nH = strlen(szHome);
    szRest = (szPattern[1] == '/') ? szPattern + 1 : "";
    nR = strlen(szRest);
    if (nH + nR + 1 > cb) {
        return -1;
    }
    memcpy(szOut, szHome, nH);
    memcpy(szOut + nH, szRest, nR + 1);
    return 0;
}

int
glob(const char *szPattern, int nFlags,
     int (*pfnErr)(const char *, int), glob_t *pGlob)
{
    char aPat[GJ_GLOB_PATH_MAX];
    const char *szUse;
    int nRc;
    int fAny = 0;

    if (szPattern == NULL || pGlob == NULL) {
        return GLOB_NOSYS;
    }

    if ((nFlags & GLOB_APPEND) == 0) {
        pGlob->gl_pathc = 0;
        pGlob->gl_pathv = NULL;
        if ((nFlags & GLOB_DOOFFS) == 0) {
            pGlob->gl_offs = 0;
        }
    }
    pGlob->gl_flags = nFlags;

    szUse = szPattern;
    if ((nFlags & GLOB_TILDE) != 0 && szPattern[0] == '~') {
        if (expand_tilde(szPattern, aPat, sizeof(aPat)) == 0) {
            szUse = aPat;
        }
    }

    if (!has_meta(szUse)) {
        if (access(szUse, F_OK) == 0) {
            return glob_append(pGlob, szUse, nFlags);
        }
        if ((nFlags & GLOB_NOCHECK) != 0) {
            return glob_append(pGlob, szPattern, nFlags);
        }
        return GLOB_NOMATCH;
    }

    nRc = glob_seg("", szUse, nFlags, pfnErr, pGlob, &fAny);
    if (nRc != 0) {
        return nRc;
    }
    if (!fAny) {
        if ((nFlags & GLOB_NOCHECK) != 0) {
            return glob_append(pGlob, szPattern, nFlags);
        }
        return GLOB_NOMATCH;
    }

    if ((nFlags & GLOB_NOSORT) == 0 && pGlob->gl_pathc > 1) {
        size_t nOff = (nFlags & GLOB_DOOFFS) ? pGlob->gl_offs : 0;
        size_t i;
        size_t j;

        for (i = 1; i < pGlob->gl_pathc; i++) {
            char *pT = pGlob->gl_pathv[nOff + i];

            j = i;
            while (j > 0 &&
                   strcmp(pGlob->gl_pathv[nOff + j - 1], pT) > 0) {
                pGlob->gl_pathv[nOff + j] = pGlob->gl_pathv[nOff + j - 1];
                j--;
            }
            pGlob->gl_pathv[nOff + j] = pT;
        }
    }
    return 0;
}

void
globfree(glob_t *pGlob)
{
    size_t i;
    size_t nOff;

    if (pGlob == NULL || pGlob->gl_pathv == NULL) {
        return;
    }
    nOff = (pGlob->gl_flags & GLOB_DOOFFS) ? pGlob->gl_offs : 0;
    for (i = 0; i < pGlob->gl_pathc; i++) {
        free(pGlob->gl_pathv[nOff + i]);
        pGlob->gl_pathv[nOff + i] = NULL;
    }
    free(pGlob->gl_pathv);
    pGlob->gl_pathv = NULL;
    pGlob->gl_pathc = 0;
}
