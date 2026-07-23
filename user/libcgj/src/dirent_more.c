/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * dirfd / readdir_r / seekdir / telldir / versionsort / scandirat.
 */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
dirfd(DIR *pDir)
{
    if (pDir == NULL) {
        errno = EINVAL;
        return -1;
    }
    return pDir->nFd;
}

int
readdir_r(DIR *pDir, struct dirent *pEntry, struct dirent **ppResult)
{
    struct dirent *p;

    if (pDir == NULL || pEntry == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = readdir(pDir);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pEntry = *p;
    *ppResult = pEntry;
    return 0;
}

void
seekdir(DIR *pDir, long nLoc)
{
    if (pDir == NULL) {
        return;
    }
    /* Bring-up: store offset in nPos; full stream seek not supported */
    if (nLoc <= 0) {
        rewinddir(pDir);
        return;
    }
    pDir->nPos = (int)nLoc;
    if (pDir->nPos > pDir->nEnd) {
        pDir->nPos = pDir->nEnd;
    }
}

long
telldir(DIR *pDir)
{
    if (pDir == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (long)pDir->nPos;
}

int
versionsort(const struct dirent **ppA, const struct dirent **ppB)
{
    if (ppA == NULL || ppB == NULL || *ppA == NULL || *ppB == NULL) {
        return 0;
    }
    return strverscmp((*ppA)->d_name, (*ppB)->d_name);
}

int
scandirat(int nDfd, const char *szPath, struct dirent ***ppList,
          int (*pfnFilter)(const struct dirent *),
          int (*pfnCmp)(const struct dirent **, const struct dirent **))
{
    DIR *pDir;
    struct dirent *pEnt;
    struct dirent **aList = NULL;
    struct dirent *pCopy;
    size_t nCap = 0;
    size_t nCnt = 0;
    size_t i;
    int nFd;

    if (szPath == NULL || ppList == NULL) {
        errno = EINVAL;
        return -1;
    }
    nFd = openat(nDfd, szPath, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (nFd < 0) {
        return -1;
    }
    pDir = fdopendir(nFd);
    if (pDir == NULL) {
        (void)close(nFd);
        return -1;
    }
    while ((pEnt = readdir(pDir)) != NULL) {
        if (pfnFilter != NULL && pfnFilter(pEnt) == 0) {
            continue;
        }
        if (nCnt + 1 > nCap) {
            size_t nNew = (nCap == 0) ? 8 : nCap * 2;
            struct dirent **aNew =
                (struct dirent **)malloc(nNew * sizeof(struct dirent *));

            if (aNew == NULL) {
                goto fail;
            }
            for (i = 0; i < nCnt; i++) {
                aNew[i] = aList[i];
            }
            free(aList);
            aList = aNew;
            nCap = nNew;
        }
        pCopy = (struct dirent *)malloc(sizeof(struct dirent));
        if (pCopy == NULL) {
            goto fail;
        }
        *pCopy = *pEnt;
        aList[nCnt++] = pCopy;
    }
    (void)closedir(pDir);
    pDir = NULL;
    if (pfnCmp != NULL && nCnt > 1) {
        qsort(aList, nCnt, sizeof(struct dirent *),
              (int (*)(const void *, const void *))pfnCmp);
    }
    *ppList = aList;
    return (int)nCnt;

fail:
    if (pDir != NULL) {
        (void)closedir(pDir);
    }
    if (aList != NULL) {
        for (i = 0; i < nCnt; i++) {
            free(aList[i]);
        }
        free(aList);
    }
    errno = ENOMEM;
    return -1;
}
