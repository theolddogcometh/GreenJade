/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft FTS: iterative DFS via readdir (NOCHDIR). PHYSICAL/LOGICAL, XDEV,
 * SEEDOT, NOSTAT, SKIP/FOLLOW/AGAIN, soft fts_children. Clean-room pure C.
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
#define GJ_FTS_NAME_MAX  256

struct gj_fts_frame {
    DIR        *pDir;
    char        szPath[GJ_FTS_PATH_MAX];
    struct stat st;
    int         nLevel;
    int         fHaveStat;
    int         fPreDone;
    int         fOpened; /* 0=not, 1=dir open, 2=leaf yielded */
    int         fIsDir;
    int         fSkip;
    int         fFollow; /* one-shot follow symlink */
    dev_t       dev;
};

struct gj_fts_priv {
    struct gj_fts_frame aStack[GJ_FTS_STACK_MAX];
    int                 nTop;
    char              **ppRoots;
    int                 cRoots;
    int                 iRoot;
    int                 fDone;
    int                 fSkipChildren;
    int                 fAgain;
    int                 nOptions;
    dev_t               devRoot;
    int                 fHaveRootDev;
    FTSENT              ent;
    struct stat         stEnt;
    char                szAcc[GJ_FTS_PATH_MAX];
    char                szPath[GJ_FTS_PATH_MAX];
    char                szName[GJ_FTS_NAME_MAX];
    FTSENT             *pChildren; /* soft fts_children list head */
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

    if (nD + nN + (fSlash ? 1u : 2u) > cb) {
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
    size_t n;
    size_t i;

    if (szDst == NULL || cb == 0) {
        return;
    }
    if (szSrc == NULL) {
        szDst[0] = '\0';
        return;
    }
    n = strlen(szSrc);
    if (n + 1 > cb) {
        n = cb - 1;
    }
    for (i = 0; i < n; i++) {
        szDst[i] = szSrc[i];
    }
    szDst[n] = '\0';
}

static int
do_stat(const char *szPath, struct stat *pSt, int nOptions, int fFollow)
{
    int fLogical = (nOptions & FTS_LOGICAL) != 0;
    int fPhysical = (nOptions & FTS_PHYSICAL) != 0;

    if (fFollow) {
        return stat(szPath, pSt);
    }
    if (fLogical && !fPhysical) {
        return stat(szPath, pSt);
    }
    return lstat(szPath, pSt);
}

static FTSENT *
fill_ent(struct gj_fts_priv *p, const char *szPath, int nLevel,
         unsigned short uInfo, const struct stat *pSt, int nErr)
{
    const char *szBase = basename_like(szPath);
    size_t nPath = strlen(szPath);
    size_t nName = strlen(szBase);
    size_t i;

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
    /* Soft: fts_name is a 1-byte flexible stub; copy first char only.
     * Full basename is in fts_path (suffix) / private szName. */
    p->ent.fts_name[0] = p->szName[0];
    for (i = 1; i < p->ent.fts_namelen && i < sizeof(p->szName); i++) {
        /* cannot extend fts_name without realloc; keep path authoritative */
        (void)i;
        break;
    }
    if (pSt != NULL) {
        p->stEnt = *pSt;
        p->ent.fts_statp = &p->stEnt;
        p->ent.fts_ino = pSt->st_ino;
        p->ent.fts_dev = pSt->st_dev;
        p->ent.fts_nlink = pSt->st_nlink;
    } else {
        p->ent.fts_statp = NULL;
    }
    p->ent.fts_pointer = p->szName; /* soft: full basename pointer */
    return &p->ent;
}

static int
push_path(struct gj_fts_priv *p, const char *szPath, int nLevel, int fFollow)
{
    struct gj_fts_frame *pFr;
    int nOptions = p->nOptions;

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
    pFr->fFollow = fFollow;
    if ((nOptions & FTS_NOSTAT) != 0 && !fFollow) {
        pFr->fHaveStat = 0;
        pFr->fIsDir = 1; /* soft: assume dir so children may be opened */
    } else {
        pFr->fHaveStat =
            (do_stat(szPath, &pFr->st, nOptions, fFollow) == 0) ? 1 : 0;
        pFr->fIsDir =
            (pFr->fHaveStat && S_ISDIR(pFr->st.st_mode)) ? 1 : 0;
        if (pFr->fHaveStat) {
            pFr->dev = pFr->st.st_dev;
        }
    }
    pFr->fPreDone = 0;
    pFr->fOpened = 0;
    pFr->fSkip = 0;
    pFr->pDir = NULL;
    return 0;
}

static void
free_children(struct gj_fts_priv *p)
{
    FTSENT *pE;
    FTSENT *pN;

    pE = p->pChildren;
    while (pE != NULL) {
        pN = pE->fts_link;
        free(pE);
        pE = pN;
    }
    p->pChildren = NULL;
}

FTS *
fts_open(char *const *ppPathv, int nOptions,
         int (*pfnCompar)(const FTSENT **, const FTSENT **))
{
    FTS *pFts;
    struct gj_fts_priv *pPriv;
    int c;
    int i;

    (void)pfnCompar; /* soft: visit roots in given order */
    if (ppPathv == NULL || ppPathv[0] == NULL) {
        errno = EINVAL;
        return NULL;
    }
    /* Default to PHYSICAL if neither LOGICAL nor PHYSICAL given. */
    if ((nOptions & (FTS_LOGICAL | FTS_PHYSICAL)) == 0) {
        nOptions |= FTS_PHYSICAL;
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
    pPriv->nOptions = nOptions | FTS_NOCHDIR;
    pPriv->fHaveRootDev = 0;
    pFts->fts_options = pPriv->nOptions;
    pFts->fts_compar = (int (*)(const FTSENT **, const FTSENT **))pfnCompar;
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
    int nOptions;

    if (pFts == NULL || pFts->fts_priv == NULL) {
        errno = EINVAL;
        return NULL;
    }
    p = (struct gj_fts_priv *)pFts->fts_priv;
    nOptions = p->nOptions;
    if (p->fDone) {
        return NULL;
    }

    /* FTS_AGAIN: re-yield current without advancing. */
    if (p->fAgain && pFts->fts_cur != NULL) {
        p->fAgain = 0;
        return pFts->fts_cur;
    }

    for (;;) {
        if (p->nTop < 0) {
            int fFollowRoot = (nOptions & FTS_COMFOLLOW) != 0;

            if (p->iRoot >= p->cRoots) {
                p->fDone = 1;
                return NULL;
            }
            if (push_path(p, p->ppRoots[p->iRoot], 0, fFollowRoot) != 0) {
                p->iRoot++;
                continue;
            }
            p->iRoot++;
        }

        pFr = &p->aStack[p->nTop];

        if (pFr->fSkip) {
            if (pFr->pDir != NULL) {
                closedir(pFr->pDir);
                pFr->pDir = NULL;
            }
            p->nTop--;
            continue;
        }

        /* Pre-order visit once */
        if (!pFr->fPreDone) {
            unsigned short uInfo;

            pFr->fPreDone = 1;
            if ((nOptions & FTS_NOSTAT) != 0 && !pFr->fHaveStat) {
                pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_NSOK, NULL, 0);
                pFr->fOpened = 1; /* try open as dir */
                pFts->fts_cur = pEnt;
                return pEnt;
            }
            if (!pFr->fHaveStat) {
                pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_NS, NULL,
                                errno);
                p->nTop--;
                pFts->fts_cur = pEnt;
                return pEnt;
            }
            if (S_ISLNK(pFr->st.st_mode) &&
                (nOptions & FTS_PHYSICAL) != 0 && !pFr->fFollow) {
                /* Dangling check soft. */
                struct stat stT;

                if (stat(pFr->szPath, &stT) != 0) {
                    uInfo = FTS_SLNONE;
                } else {
                    uInfo = FTS_SL;
                }
            } else if (pFr->fIsDir) {
                uInfo = FTS_D;
                if (!p->fHaveRootDev) {
                    p->devRoot = pFr->st.st_dev;
                    p->fHaveRootDev = 1;
                }
            } else if (S_ISREG(pFr->st.st_mode)) {
                uInfo = FTS_F;
            } else {
                uInfo = FTS_DEFAULT;
            }
            pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, uInfo, &pFr->st, 0);
            if (!pFr->fIsDir && uInfo != FTS_D) {
                pFr->fOpened = 2;
            }
            pFts->fts_cur = pEnt;
            return pEnt;
        }

        if (pFr->fOpened == 2) {
            p->nTop--;
            continue;
        }

        if (pFr->fIsDir || ((nOptions & FTS_NOSTAT) != 0 && pFr->fOpened)) {
            /* XDEV: skip descent across devices. */
            if ((nOptions & FTS_XDEV) != 0 && p->fHaveRootDev &&
                pFr->fHaveStat && pFr->st.st_dev != p->devRoot) {
                pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DP,
                                pFr->fHaveStat ? &pFr->st : NULL, 0);
                p->nTop--;
                pFts->fts_cur = pEnt;
                return pEnt;
            }
            if (p->fSkipChildren) {
                p->fSkipChildren = 0;
                pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DP,
                                pFr->fHaveStat ? &pFr->st : NULL, 0);
                if (pFr->pDir != NULL) {
                    closedir(pFr->pDir);
                    pFr->pDir = NULL;
                }
                p->nTop--;
                pFts->fts_cur = pEnt;
                return pEnt;
            }
            if (!pFr->fOpened || pFr->fOpened == 1) {
                if (pFr->pDir == NULL && pFr->fOpened != 2) {
                    pFr->pDir = opendir(pFr->szPath);
                    pFr->fOpened = 1;
                    if (pFr->pDir == NULL) {
                        pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DNR,
                                        pFr->fHaveStat ? &pFr->st : NULL,
                                        errno);
                        p->nTop--;
                        pFts->fts_cur = pEnt;
                        return pEnt;
                    }
                }
            }
            if (pFr->pDir != NULL) {
                struct dirent *pDe;
                int fPushed = 0;
                int fSeedot = (nOptions & FTS_SEEDOT) != 0;

                while ((pDe = readdir(pFr->pDir)) != NULL) {
                    char aChild[GJ_FTS_PATH_MAX];

                    if (pDe->d_name[0] == '.' &&
                        (pDe->d_name[1] == '\0' ||
                         (pDe->d_name[1] == '.' &&
                          pDe->d_name[2] == '\0'))) {
                        if (!fSeedot) {
                            continue;
                        }
                    }
                    if (path_join(aChild, sizeof(aChild), pFr->szPath,
                                  pDe->d_name) != 0) {
                        continue;
                    }
                    if (push_path(p, aChild, pFr->nLevel + 1, 0) == 0) {
                        fPushed = 1;
                        break;
                    }
                }
                if (fPushed) {
                    continue;
                }
                closedir(pFr->pDir);
                pFr->pDir = NULL;
            }
            pEnt = fill_ent(p, pFr->szPath, pFr->nLevel, FTS_DP,
                            pFr->fHaveStat ? &pFr->st : NULL, 0);
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
    struct gj_fts_frame *pFr;

    if (pFts == NULL || pFts->fts_priv == NULL || pEnt == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = (struct gj_fts_priv *)pFts->fts_priv;
    pEnt->fts_instr = (unsigned short)nInstr;
    if (nInstr == FTS_SKIP) {
        p->fSkipChildren = 1;
        if (p->nTop >= 0) {
            p->aStack[p->nTop].fSkip = 1;
        }
    } else if (nInstr == FTS_AGAIN) {
        p->fAgain = 1;
    } else if (nInstr == FTS_FOLLOW) {
        if (p->nTop >= 0) {
            pFr = &p->aStack[p->nTop];
            pFr->fFollow = 1;
            pFr->fHaveStat =
                (do_stat(pFr->szPath, &pFr->st, p->nOptions, 1) == 0) ? 1 : 0;
            pFr->fIsDir =
                (pFr->fHaveStat && S_ISDIR(pFr->st.st_mode)) ? 1 : 0;
            pFr->fPreDone = 0;
            pFr->fOpened = 0;
        }
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
        free_children(p);
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
    struct gj_fts_priv *p;
    struct gj_fts_frame *pFr;
    DIR *pDir;
    struct dirent *pDe;
    FTSENT *pHead = NULL;
    FTSENT *pTail = NULL;
    const char *szPath;

    (void)nOptions;
    if (pFts == NULL || pFts->fts_priv == NULL) {
        errno = EINVAL;
        return NULL;
    }
    p = (struct gj_fts_priv *)pFts->fts_priv;
    free_children(p);

    if (p->nTop < 0) {
        /* Before first fts_read: list roots as soft children. */
        int i;

        for (i = 0; i < p->cRoots; i++) {
            size_t nName = strlen(p->ppRoots[i]);
            FTSENT *pE =
                (FTSENT *)calloc(1, sizeof(FTSENT) + nName + 1);

            if (pE == NULL) {
                free_children(p);
                errno = ENOMEM;
                return NULL;
            }
            memcpy(pE->fts_name, p->ppRoots[i], nName + 1);
            pE->fts_namelen = nName;
            pE->fts_path = pE->fts_name;
            pE->fts_accpath = pE->fts_name;
            pE->fts_pathlen = nName;
            pE->fts_level = 0;
            pE->fts_info = FTS_INIT;
            pE->fts_link = NULL;
            if (pTail == NULL) {
                pHead = pE;
            } else {
                pTail->fts_link = pE;
            }
            pTail = pE;
        }
        p->pChildren = pHead;
        pFts->fts_child = pHead;
        return pHead;
    }

    pFr = &p->aStack[p->nTop];
    szPath = pFr->szPath;
    if (!pFr->fIsDir && (p->nOptions & FTS_NOSTAT) == 0) {
        pFts->fts_child = NULL;
        return NULL;
    }
    pDir = opendir(szPath);
    if (pDir == NULL) {
        pFts->fts_child = NULL;
        return NULL;
    }
    while ((pDe = readdir(pDir)) != NULL) {
        size_t nName;
        FTSENT *pE;
        char aChild[GJ_FTS_PATH_MAX];
        struct stat st;
        int fSeedot = (p->nOptions & FTS_SEEDOT) != 0;

        if (pDe->d_name[0] == '.' &&
            (pDe->d_name[1] == '\0' ||
             (pDe->d_name[1] == '.' && pDe->d_name[2] == '\0'))) {
            if (!fSeedot) {
                continue;
            }
        }
        nName = strlen(pDe->d_name);
        pE = (FTSENT *)calloc(1, sizeof(FTSENT) + nName + 1);
        if (pE == NULL) {
            closedir(pDir);
            free_children(p);
            errno = ENOMEM;
            return NULL;
        }
        memcpy(pE->fts_name, pDe->d_name, nName + 1);
        pE->fts_namelen = nName;
        pE->fts_level = (short)(pFr->nLevel + 1);
        if (path_join(aChild, sizeof(aChild), szPath, pDe->d_name) == 0) {
            if (do_stat(aChild, &st, p->nOptions, 0) == 0) {
                /* Soft: no per-child stat buffer; type only. */
                if (S_ISDIR(st.st_mode)) {
                    pE->fts_info = FTS_D;
                } else if (S_ISLNK(st.st_mode)) {
                    pE->fts_info = FTS_SL;
                } else if (S_ISREG(st.st_mode)) {
                    pE->fts_info = FTS_F;
                } else {
                    pE->fts_info = FTS_DEFAULT;
                }
                pE->fts_ino = st.st_ino;
                pE->fts_dev = st.st_dev;
                pE->fts_nlink = st.st_nlink;
            } else {
                pE->fts_info = FTS_NS;
                pE->fts_errno = errno;
            }
        } else {
            pE->fts_info = FTS_NS;
        }
        pE->fts_link = NULL;
        if (pTail == NULL) {
            pHead = pE;
        } else {
            pTail->fts_link = pE;
        }
        pTail = pE;
    }
    closedir(pDir);
    p->pChildren = pHead;
    pFts->fts_child = pHead;
    return pHead;
}
