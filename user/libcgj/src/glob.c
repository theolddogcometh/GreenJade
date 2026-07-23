/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Basic glob: directory + fnmatch for * and ?. No brace/tilde expand.
 */
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
path_join(char *szOut, size_t cb, const char *szDir, const char *szName)
{
    size_t nD = (szDir != NULL) ? strlen(szDir) : 0;
    size_t nN = (szName != NULL) ? strlen(szName) : 0;
    size_t i = 0;
    size_t j;

    if (cb == 0) {
        return -1;
    }
    for (j = 0; j < nD && i + 1 < cb; j++) {
        szOut[i++] = szDir[j];
    }
    if (nD > 0 && szDir[nD - 1] != '/' && i + 1 < cb) {
        szOut[i++] = '/';
    }
    for (j = 0; j < nN && i + 1 < cb; j++) {
        szOut[i++] = szName[j];
    }
    szOut[i] = '\0';
    return 0;
}

static int
glob_append(glob_t *pG, const char *szPath)
{
    char **pNew;
    char *sz;
    size_t n = pG->gl_pathc;
    size_t nOff = (pG->gl_flags & GLOB_DOOFFS) ? pG->gl_offs : 0;
    size_t nSlots = nOff + n + 2; /* +1 new + NULL */

    sz = strdup(szPath);
    if (sz == NULL) {
        return GLOB_NOSPACE;
    }
    pNew = (char **)realloc(pG->gl_pathv, nSlots * sizeof(char *));
    if (pNew == NULL) {
        free(sz);
        return GLOB_NOSPACE;
    }
    pG->gl_pathv = pNew;
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
    while (sz != NULL && *sz != '\0') {
        if (*sz == '*' || *sz == '?' || *sz == '[') {
            return 1;
        }
        sz++;
    }
    return 0;
}

int
glob(const char *szPattern, int nFlags,
     int (*pfnErr)(const char *, int), glob_t *pGlob)
{
    char aDir[256];
    char aPat[128];
    const char *pSlash;
    DIR *pDir;
    struct dirent *pEnt;
    int nFnFlags = 0;
    int nRc = 0;
    int fAny = 0;

    (void)pfnErr;
    if (szPattern == NULL || pGlob == NULL) {
        return GLOB_NOSYS;
    }
    if ((nFlags & GLOB_APPEND) == 0) {
        pGlob->gl_pathc = 0;
        pGlob->gl_pathv = NULL;
        pGlob->gl_offs = 0;
    }
    pGlob->gl_flags = nFlags;

    if (!has_meta(szPattern)) {
        /* Literal path: include if exists (access) or GLOB_NOCHECK */
        if (access(szPattern, F_OK) == 0 || (nFlags & GLOB_NOCHECK) != 0) {
            nRc = glob_append(pGlob, szPattern);
            return nRc;
        }
        return (nFlags & GLOB_NOCHECK) ? 0 : GLOB_NOMATCH;
    }

    pSlash = strrchr(szPattern, '/');
    if (pSlash != NULL) {
        size_t nD = (size_t)(pSlash - szPattern);

        if (nD >= sizeof(aDir)) {
            nD = sizeof(aDir) - 1;
        }
        if (nD == 0) {
            aDir[0] = '/';
            aDir[1] = '\0';
        } else {
            memcpy(aDir, szPattern, nD);
            aDir[nD] = '\0';
        }
        {
            size_t nP = strlen(pSlash + 1);

            if (nP >= sizeof(aPat)) {
                nP = sizeof(aPat) - 1;
            }
            memcpy(aPat, pSlash + 1, nP);
            aPat[nP] = '\0';
        }
    } else {
        aDir[0] = '.';
        aDir[1] = '\0';
        {
            size_t nP = strlen(szPattern);

            if (nP >= sizeof(aPat)) {
                nP = sizeof(aPat) - 1;
            }
            memcpy(aPat, szPattern, nP);
            aPat[nP] = '\0';
        }
    }

    if ((nFlags & GLOB_NOESCAPE) != 0) {
        nFnFlags |= FNM_NOESCAPE;
    }

    pDir = opendir(aDir);
    if (pDir == NULL) {
        if ((nFlags & GLOB_ERR) != 0) {
            return GLOB_ABORTED;
        }
        if ((nFlags & GLOB_NOCHECK) != 0) {
            return glob_append(pGlob, szPattern);
        }
        return GLOB_NOMATCH;
    }

    while ((pEnt = readdir(pDir)) != NULL) {
        char aPath[384];
        const char *pAdd;

        /* Skip . and .. unless pattern explicitly wants them */
        if (pEnt->d_name[0] == '.' &&
            (pEnt->d_name[1] == '\0' ||
             (pEnt->d_name[1] == '.' && pEnt->d_name[2] == '\0'))) {
            if (aPat[0] != '.') {
                continue;
            }
        }
        if (fnmatch(aPat, pEnt->d_name, nFnFlags) != 0) {
            continue;
        }
        if (path_join(aPath, sizeof(aPath), aDir, pEnt->d_name) != 0) {
            continue;
        }
        /* Normalize ./foo → foo for cwd patterns */
        pAdd = aPath;
        if (aDir[0] == '.' && aDir[1] == '\0' && aPath[0] == '.' &&
            aPath[1] == '/') {
            pAdd = aPath + 2;
        }
        nRc = glob_append(pGlob, pAdd);
        if (nRc != 0) {
            closedir(pDir);
            return nRc;
        }
        fAny = 1;
    }
    closedir(pDir);

    if (!fAny) {
        if ((nFlags & GLOB_NOCHECK) != 0) {
            return glob_append(pGlob, szPattern);
        }
        return GLOB_NOMATCH;
    }
    /* Simple insertion sort unless GLOB_NOSORT */
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
    }
    free(pGlob->gl_pathv);
    pGlob->gl_pathv = NULL;
    pGlob->gl_pathc = 0;
}
