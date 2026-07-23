/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Light ftw/nftw: depth-first walk, no mount/chdir, limited recursion.
 */
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define GJ_FTW_MAX_DEPTH 32
#define GJ_FTW_PATH_MAX  512

typedef int (*gj_ftw_fn)(const char *, const struct stat *, int);
typedef int (*gj_nftw_fn)(const char *, const struct stat *, int, struct FTW *);

static int
path_join(char *szOut, size_t cb, const char *szDir, const char *szName)
{
    size_t nD = strlen(szDir);
    size_t nN = strlen(szName);
    size_t i = 0;
    size_t j;
    int fSlash = (nD > 0 && szDir[nD - 1] == '/');

    if (nD + nN + (fSlash ? 1 : 2) > cb) {
        return -1;
    }
    for (j = 0; j < nD; j++) {
        szOut[i++] = szDir[j];
    }
    if (!fSlash && nD > 0) {
        szOut[i++] = '/';
    }
    for (j = 0; j < nN; j++) {
        szOut[i++] = szName[j];
    }
    szOut[i] = '\0';
    return 0;
}

static int
base_of(const char *szPath)
{
    const char *p = strrchr(szPath, '/');

    if (p == NULL) {
        return 0;
    }
    return (int)(p - szPath) + 1;
}

static int
call_cb(const char *szPath, const struct stat *pSt, int nType, int nLevel,
        gj_ftw_fn pfnFtw, gj_nftw_fn pfnNftw)
{
    if (pfnNftw != NULL) {
        struct FTW ftwb;

        ftwb.base = base_of(szPath);
        ftwb.level = nLevel;
        return pfnNftw(szPath, pSt, nType, &ftwb);
    }
    return pfnFtw(szPath, pSt, nType);
}

static int
walk(const char *szPath, int nLevel, int nFlags, gj_ftw_fn pfnFtw,
     gj_nftw_fn pfnNftw)
{
    struct stat st;
    int nType;
    int nRc;
    int fPhys = (nFlags & FTW_PHYS) != 0;
    int fDepth = (nFlags & FTW_DEPTH) != 0;

    if (nLevel > GJ_FTW_MAX_DEPTH) {
        return 0;
    }
    if ((fPhys ? lstat(szPath, &st) : stat(szPath, &st)) != 0) {
        memset(&st, 0, sizeof(st));
        return call_cb(szPath, &st, FTW_NS, nLevel, pfnFtw, pfnNftw);
    }
    if (S_ISLNK(st.st_mode)) {
        nType = FTW_SL;
    } else if (S_ISDIR(st.st_mode)) {
        nType = FTW_D;
    } else {
        nType = FTW_F;
    }

    if (!fDepth) {
        nRc = call_cb(szPath, &st, nType, nLevel, pfnFtw, pfnNftw);
        if (nRc != 0) {
            return nRc;
        }
    }

    if (nType == FTW_D) {
        DIR *pDir = opendir(szPath);
        struct dirent *pEnt;

        if (pDir == NULL) {
            nRc = call_cb(szPath, &st, FTW_DNR, nLevel, pfnFtw, pfnNftw);
            if (nRc != 0) {
                return nRc;
            }
        } else {
            while ((pEnt = readdir(pDir)) != NULL) {
                char aChild[GJ_FTW_PATH_MAX];

                if (pEnt->d_name[0] == '.' &&
                    (pEnt->d_name[1] == '\0' ||
                     (pEnt->d_name[1] == '.' && pEnt->d_name[2] == '\0'))) {
                    continue;
                }
                if (path_join(aChild, sizeof(aChild), szPath, pEnt->d_name) !=
                    0) {
                    continue;
                }
                nRc = walk(aChild, nLevel + 1, nFlags, pfnFtw, pfnNftw);
                if (nRc != 0) {
                    closedir(pDir);
                    return nRc;
                }
            }
            closedir(pDir);
        }
    }

    if (fDepth) {
        nRc = call_cb(szPath, &st, (nType == FTW_D) ? FTW_DP : nType, nLevel,
                      pfnFtw, pfnNftw);
        if (nRc != 0) {
            return nRc;
        }
    }
    return 0;
}

int
ftw(const char *szPath, int (*pfn)(const char *, const struct stat *, int),
    int nFdLimit)
{
    (void)nFdLimit;
    if (szPath == NULL || pfn == NULL) {
        errno = EINVAL;
        return -1;
    }
    return walk(szPath, 0, 0, pfn, NULL);
}

int
nftw(const char *szPath,
     int (*pfn)(const char *, const struct stat *, int, struct FTW *),
     int nFdLimit, int nFlags)
{
    (void)nFdLimit;
    if (szPath == NULL || pfn == NULL) {
        errno = EINVAL;
        return -1;
    }
    return walk(szPath, 0, nFlags, NULL, pfn);
}
