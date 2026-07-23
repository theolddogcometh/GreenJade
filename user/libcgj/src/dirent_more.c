/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft dirfd / readdir_r / seekdir / telldir / versionsort / scandirat.
 * seekdir/telldir use Linux d_off cookies via lseek on the dir fd.
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
    if (pDir->nFd < 0) {
        errno = EBADF;
        return -1;
    }
    return pDir->nFd;
}

int
readdir_r(DIR *pDir, struct dirent *pEntry, struct dirent **ppResult)
{
    struct dirent *p;
    int nSaved;

    if (pDir == NULL || pEntry == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nSaved = errno;
    errno = 0;
    p = readdir(pDir);
    if (p == NULL) {
        *ppResult = NULL;
        /* EOF: return 0 and restore errno; real error: return errno. */
        if (errno == 0) {
            errno = nSaved;
            return 0;
        }
        {
            int nErr = errno;

            errno = nSaved;
            return nErr;
        }
    }
    *pEntry = *p;
    *ppResult = pEntry;
    errno = nSaved;
    return 0;
}

void
seekdir(DIR *pDir, long nLoc)
{
    if (pDir == NULL || pDir->nFd < 0) {
        return;
    }
    if (nLoc <= 0) {
        (void)lseek(pDir->nFd, 0, SEEK_SET);
        pDir->nPos = 0;
        pDir->nEnd = 0;
        return;
    }
    /*
     * Soft Linux: directory cookie is d_off from getdents64. Seeking the
     * dir fd and dropping the buffer resumes readdir at that cookie.
     */
    if (lseek(pDir->nFd, (off_t)nLoc, SEEK_SET) == (off_t)-1) {
        /* Fall back to rewind on bad cookie. */
        (void)lseek(pDir->nFd, 0, SEEK_SET);
        pDir->nPos = 0;
        pDir->nEnd = 0;
        return;
    }
    pDir->nPos = 0;
    pDir->nEnd = 0;
}

long
telldir(DIR *pDir)
{
    if (pDir == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (pDir->nFd < 0) {
        errno = EBADF;
        return -1;
    }
    /*
     * After readdir, ent.d_off is the kernel cookie for the next entry.
     * At start-of-stream (empty buffer), cookie 0.
     */
    if (pDir->nEnd == 0 && pDir->nPos == 0) {
        return 0;
    }
    return (long)pDir->ent.d_off;
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
    int nSaved;

    if (szPath == NULL || ppList == NULL) {
        errno = EINVAL;
        return -1;
    }
    *ppList = NULL;
    nFd = openat(nDfd, szPath, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (nFd < 0) {
        return -1;
    }
    pDir = fdopendir(nFd);
    if (pDir == NULL) {
        (void)close(nFd);
        return -1;
    }
    nSaved = errno;
    while ((pEnt = readdir(pDir)) != NULL) {
        if (pfnFilter != NULL && pfnFilter(pEnt) == 0) {
            continue;
        }
        if (nCnt + 1 > nCap) {
            size_t nNew = (nCap == 0) ? 8 : nCap * 2;
            struct dirent **aNew =
                (struct dirent **)realloc(aList, nNew * sizeof(struct dirent *));

            if (aNew == NULL) {
                goto fail;
            }
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
        /* Soft insertion sort — avoids qsort fn-pointer cast UB. */
        for (i = 1; i < nCnt; i++) {
            struct dirent *pT = aList[i];
            size_t j = i;

            while (j > 0) {
                const struct dirent *pL = aList[j - 1];
                const struct dirent *pR = pT;

                if (pfnCmp(&pL, &pR) <= 0) {
                    break;
                }
                aList[j] = aList[j - 1];
                j--;
            }
            aList[j] = pT;
        }
    }
    *ppList = aList;
    errno = nSaved;
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
