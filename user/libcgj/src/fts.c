/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal FTS: iterative DFS via readdir (no chdir).
 */
#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define GJ_FTS_PATH_MAX  512
#define GJ_FTS_STACK_MAX 64

struct gj_fts_frame {
    DIR        *pDir;
    char        szPath[GJ_FTS_PATH_MAX];
    struct stat st;
    int         nLevel;
    int         fHaveStat;
    int         fPreDone;
    int         fOpened;
    int         fIsDir;
};

struct gj_fts_priv {
    struct gj_fts_frame aStack[GJ_FTS_STACK_MAX];
    int                 nTop;
    char              **ppRoots;
    int                 cRoots;
    int                 iRoot;
    int                 fDone;
    int                 fSkipChildren;
    FTSENT              ent;
    struct stat         stEnt;
    char                szAcc[GJ_FTS_PATH_MAX];
    char                szPath[GJ_FTS_PATH_MAX];
    char                szName[256];
};

static const char *
basename_like(const char *szPath)
{
    const char *p = strrchr(szPath, '/');

    if (p == NULL || p[1] == '\0') {
        return szPath;
    }
    return p + 1;
}

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

static void
copy_str(char *szDst, size_t cb, const char *szSrc)
{
    size_t n = strlen(szSrc);
    size_t i;

    if (n + 1 > cb) {
        n = cb - 1;
    }
    for (i = 0; i < n; i++) {
        szDst[i] = szSrc[i];
    }
    szDst[n] = '\0';
}

static FTSENT *
fill_ent(struct gj_fts_priv *p, const char *szPath, int nLevel,
         unsigned short uInfo, const struct stat *pSt, int nErr)
{
    const char *szBase = basename_like(szPath);
    size_t nPath = strlen(szPath);
    size_t nName = strlen(szBase);

    copy_str(p->szPath, sizeof(p->szPath), szPath);
    copy_str(p->szAcc, sizeof(p->szAcc), szPath);
    copy_str(p->szName, sizeof(p->szName), szBase);

    memset(&p->ent, 0, sizeof(p->ent));
    p->ent.fts_path = p->szPath;
    p->ent.fts_accpath = p->szAcc;
    p->ent.fts_pathlen = nPath < sizeof(p->szPath) ? nPath
                                                   : sizeof(p->szPath) - 1;
    p->ent.fts_namelen = nName < sizeof(p->szName) ? nName
                                                   : sizeof(p->szName) - 1;
    p->ent.fts_level = (short)nLevel;
    p->ent.fts_info = uInfo;
    p->ent.fts_errno = nErr;
    p->ent.fts_name[0] = p->szName[0];
    if (pSt != NULL) {
        p->stEnt = *pSt;
        p->ent.fts_statp = &p->stEnt;
        p->ent.fts_ino = pSt->st_ino;
        p->ent.fts_dev = pSt->st_dev;
        p->ent.fts_nlink = pSt->st_nlink;
    } else {
        p->ent.fts_statp = NULL;
    }
    return &p->ent;
}

static int
push_path(struct gj_fts_priv *p, const char *szPath, int nLevel)
{
    struct gj_fts_frame *pFr;

    if (p->nTop + 1 >= GJ_FTS_STACK_MAX) {
        errno = ELOOP;
        return -1;
    }
    if (strlen(szPath) >= GJ_FTS_PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    p->nTop++;
    pFr = &p->aStack[p->nTop];
    memset(pFr, 0, sizeof(*pFr));
    copy_str(pFr->szPath, sizeof(pFr->szPath), szPath);
    pFr->nLevel = nLevel;
    pFr->fHaveStat = (lstat(szPath, &pFr->st) == 0) ? 1 : 0;
    pFr->fIsDir = (pFr->fHaveStat && S_ISDIR(pFr->st.st_mode)) ? 1 : 0;
    pFr->fPreDone = 0;
    pFr->fOpened = 0;
    pFr->pDir = NULL;
    return 0;
}

FTS *
fts_open(char *const *ppPathv, int nOptions,
         int (*pfnCompar)(const FTSENT **, const FTSENT **))
{
    FTS *pFts;
    struct gj_fts_priv *pPriv;
    int c;
    int i;

    (void)pfnCompar;
    if (ppPathv == NULL || ppPathv[0] == NULL) {
        errno = EINVAL;
        return NULL;
    }
    pFts = (FTS *)calloc(1, sizeof(*pFts));
    if (pFts == NULL) {
        return NULL;
    }
    pPriv = (struct gj_fts_priv *)calloc(1, sizeof(*pPriv));
    if (pPriv == NULL) {
        free(pFts);
        return NULL;
    }
    for (c = 0; ppPathv[c] != NULL; c++) {
    }
    pPriv->ppRoots = (char **)calloc((size_t)c + 1, sizeof(char *));
    if (pPriv->ppRoots == NULL) {
        free(pPriv);
        free(pFts);
        return NULL;
    }
    for (i = 0; i < c; i++) {
        pPriv->ppRoots[i] = strdup(ppPathv[i]);
        if (pPriv->ppRoots[i] == NULL) {
            int j;

            for (j = 0; j < i; j++) {
                free(pPriv->ppRoots[j]);
            }
            free(pPriv->ppRoots);
            free(pPriv);
            free(pFts);
            return NULL;
        }
    }
    pPriv->cRoots = c;
    pPriv->iRoot = 0;
    pPriv->nTop = -1;
    pPriv->fDone = 0;
    pFts->fts_options = nOptions | FTS_NOCHDIR;
    pFts->fts_priv = pPriv;
    pFts->fts_path = pPriv->szPath;
    return pFts;
}

FTSENT *
fts_read(FTS *pFts)
{
    struct gj_fts_priv *p;
    struct gj_fts_frame *pFr;
    FTSENT *pEnt;

    if (pFts == NULL || pFts->fts_priv == NULL) {
        errno = EINVAL;
        return NULL;
    }
    p = (struct gj_fts_priv *)pFts->fts_priv;
    if (p->fDone) {
        return NULL;
    }

    for (;;) {
        if (p->nTop < 0) {
            if (p->iRoot >= p->cRoots) {
                p->fDone = 1;
                return NULL;
            }
            if (push_path(p, p->ppRoots[p->iRoot], 0) != 0) {
                p->iRoot++;
                continue;
            }
            p->iRoot++;
        }

        pFr = &p->aStack[p->nTop];

        /* Pre-order visit once */
        if (!pFr->fPreDone) {
            unsigned short uInfo;

            pFr->fPreDone = 1;
            if (!pFr->fHaveStat) {
                pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_NS, NULL,
                                errno);
                p->nTop--;
                pFts->fts_cur = pEnt;
                return pEnt;
            }
            if (S_ISLNK(pFr->st.st_mode)) {
                uInfo = FTS_SL;
            } else if (pFr->fIsDir) {
                uInfo = FTS_D;
            } else {
                uInfo = FTS_F;
            }
            pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, uInfo, &pFr->st, 0);
            if (!pFr->fIsDir) {
                /* leaf: pop on next entry after yield */
                pFr->fOpened = 2; /* mark leaf yielded */
            }
            pFts->fts_cur = pEnt;
            return pEnt;
        }

        /* Leaf already yielded — pop */
        if (pFr->fOpened == 2) {
            p->nTop--;
            continue;
        }

        /* Directory: open + children or postorder */
        if (pFr->fIsDir) {
            if (p->fSkipChildren) {
                p->fSkipChildren = 0;
                pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DP, &pFr->st,
                                0);
                if (pFr->pDir != NULL) {
                    closedir(pFr->pDir);
                    pFr->pDir = NULL;
                }
                p->nTop--;
                pFts->fts_cur = pEnt;
                return pEnt;
            }
            if (!pFr->fOpened) {
                pFr->pDir = opendir(pFr->szPath);
                pFr->fOpened = 1;
                if (pFr->pDir == NULL) {
                    pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DNR,
                                    &pFr->st, errno);
                    p->nTop--;
                    pFts->fts_cur = pEnt;
                    return pEnt;
                }
            }
            if (pFr->pDir != NULL) {
                struct dirent *pDe;
                int fPushed = 0;

                while ((pDe = readdir(pFr->pDir)) != NULL) {
                    char aChild[GJ_FTS_PATH_MAX];

                    if (pDe->d_name[0] == '.' &&
                        (pDe->d_name[1] == '\0' ||
                         (pDe->d_name[1] == '.' && pDe->d_name[2] == '\0'))) {
                        continue;
                    }
                    if (path_join(aChild, sizeof(aChild), pFr->szPath,
                                  pDe->d_name) != 0) {
                        continue;
                    }
                    if (push_path(p, aChild, pFr->nLevel + 1) == 0) {
                        fPushed = 1;
                        break; /* process child */
                    }
                }
                if (fPushed) {
                    continue; /* child on stack */
                }
                closedir(pFr->pDir);
                pFr->pDir = NULL;
            }
            /* postorder */
            pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DP, &pFr->st, 0);
            p->nTop--;
            pFts->fts_cur = pEnt;
            return pEnt;
        }

        p->nTop--;
    }
}

int
fts_set(FTS *pFts, FTSENT *pEnt, int nInstr)
{
    struct gj_fts_priv *p;

    if (pFts == NULL || pFts->fts_priv == NULL || pEnt == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = (struct gj_fts_priv *)pFts->fts_priv;
    pEnt->fts_instr = (unsigned short)nInstr;
    if (nInstr == FTS_SKIP) {
        p->fSkipChildren = 1;
    }
    return 0;
}

int
fts_close(FTS *pFts)
{
    struct gj_fts_priv *p;
    int i;

    if (pFts == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = (struct gj_fts_priv *)pFts->fts_priv;
    if (p != NULL) {
        while (p->nTop >= 0) {
            if (p->aStack[p->nTop].pDir != NULL) {
                closedir(p->aStack[p->nTop].pDir);
            }
            p->nTop--;
        }
        if (p->ppRoots != NULL) {
            for (i = 0; i < p->cRoots; i++) {
                free(p->ppRoots[i]);
            }
            free(p->ppRoots);
        }
        free(p);
    }
    free(pFts);
    return 0;
}

FTSENT *
fts_children(FTS *pFts, int nOptions)
{
    (void)pFts;
    (void)nOptions;
    errno = ENOSYS;
    return NULL;
}
