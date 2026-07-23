/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal working posix_spawn / posix_spawnp + attr / file_actions.
 */
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <spawn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

enum {
    GJ_SPAWN_ACT_CLOSE     = 1,
    GJ_SPAWN_ACT_DUP2      = 2,
    GJ_SPAWN_ACT_OPEN      = 3,
    GJ_SPAWN_ACT_CHDIR     = 4,
    GJ_SPAWN_ACT_FCHDIR    = 5,
    GJ_SPAWN_ACT_CLOSEFROM = 6
};

struct gj_spawn_act {
    int  nKind;
    int  nFd;
    int  nNewfd;
    int  nOflag;
    mode_t nMode;
    char *szPath;
};

struct gj_spawn_fa {
    int                 cUsed;
    int                 cAlloc;
    struct gj_spawn_act *pActs;
};

static void
fa_free(struct gj_spawn_fa *pFa)
{
    int i;

    if (pFa == NULL) {
        return;
    }
    if (pFa->pActs != NULL) {
        for (i = 0; i < pFa->cUsed; i++) {
            free(pFa->pActs[i].szPath);
            pFa->pActs[i].szPath = NULL;
        }
        free(pFa->pActs);
        pFa->pActs = NULL;
    }
    pFa->cUsed = 0;
    pFa->cAlloc = 0;
}

static int
fa_grow(struct gj_spawn_fa *pFa)
{
    int cNew;
    struct gj_spawn_act *pN;

    if (pFa->cUsed < pFa->cAlloc) {
        return 0;
    }
    cNew = pFa->cAlloc ? pFa->cAlloc * 2 : 4;
    pN = (struct gj_spawn_act *)realloc(pFa->pActs,
                                        (size_t)cNew * sizeof(*pN));
    if (pN == NULL) {
        errno = ENOMEM;
        return -1;
    }
    pFa->pActs = pN;
    pFa->cAlloc = cNew;
    return 0;
}

static void
apply_file_actions(const posix_spawn_file_actions_t *pFileActions)
{
    struct gj_spawn_fa *pFa;
    int i;

    if (pFileActions == NULL || pFileActions->__actions == NULL) {
        return;
    }
    pFa = (struct gj_spawn_fa *)pFileActions->__actions;
    for (i = 0; i < pFa->cUsed; i++) {
        struct gj_spawn_act *pA = &pFa->pActs[i];
        int nFd;

        if (pA->nKind == GJ_SPAWN_ACT_CLOSE) {
            (void)close(pA->nFd);
        } else if (pA->nKind == GJ_SPAWN_ACT_DUP2) {
            if (dup2(pA->nFd, pA->nNewfd) < 0) {
                _exit(127);
            }
        } else if (pA->nKind == GJ_SPAWN_ACT_OPEN) {
            nFd = open(pA->szPath, pA->nOflag, pA->nMode);
            if (nFd < 0) {
                _exit(127);
            }
            if (nFd != pA->nFd) {
                if (dup2(nFd, pA->nFd) < 0) {
                    _exit(127);
                }
                (void)close(nFd);
            }
        } else if (pA->nKind == GJ_SPAWN_ACT_CHDIR) {
            if (pA->szPath == NULL || chdir(pA->szPath) != 0) {
                _exit(127);
            }
        } else if (pA->nKind == GJ_SPAWN_ACT_FCHDIR) {
            if (fchdir(pA->nFd) != 0) {
                _exit(127);
            }
        } else if (pA->nKind == GJ_SPAWN_ACT_CLOSEFROM) {
            int fd;

            for (fd = pA->nFd; fd < 1024; fd++) {
                (void)close(fd);
            }
        }
    }
}

static int
do_spawn(pid_t *pPid, const char *szPath,
         const posix_spawn_file_actions_t *pFileActions,
         const posix_spawnattr_t *pAttr, char *const aArgv[],
         char *const aEnvp[], int fPathSearch)
{
    pid_t pid;
    char *const *pEnv;
    extern char **environ;

    if (szPath == NULL || aArgv == NULL) {
        errno = EINVAL;
        return errno;
    }
    pEnv = aEnvp != NULL ? aEnvp : environ;

    pid = fork();
    if (pid < 0) {
        return errno;
    }
    if (pid == 0) {
        /* Child */
        if (pAttr != NULL) {
            if ((pAttr->__flags & POSIX_SPAWN_SETPGROUP) != 0) {
                /* setpgid not yet in surface — ignore bring-up */
                (void)pAttr->__pgrp;
            }
            if ((pAttr->__flags & POSIX_SPAWN_SETSIGMASK) != 0) {
                (void)sigprocmask(SIG_SETMASK, &pAttr->__ss, NULL);
            }
        }
        apply_file_actions(pFileActions);
        if (fPathSearch) {
            /* Absolute or relative with slash: direct exec */
            if (strchr(szPath, '/') != NULL) {
                (void)execve(szPath, aArgv, pEnv);
            } else {
                const char *szPathEnv = getenv("PATH");
                char aTry[512];
                const char *p;
                const char *pColon;

                if (szPathEnv == NULL) {
                    szPathEnv = "/bin:/usr/bin";
                }
                p = szPathEnv;
                for (;;) {
                    size_t nDir;
                    size_t nFile;
                    size_t i;

                    pColon = strchr(p, ':');
                    nDir = pColon ? (size_t)(pColon - p) : strlen(p);
                    nFile = strlen(szPath);
                    if (nDir + 1 + nFile + 1 <= sizeof(aTry)) {
                        for (i = 0; i < nDir; i++) {
                            aTry[i] = p[i];
                        }
                        if (nDir == 0) {
                            aTry[0] = '.';
                            nDir = 1;
                        }
                        aTry[nDir] = '/';
                        for (i = 0; i < nFile; i++) {
                            aTry[nDir + 1 + i] = szPath[i];
                        }
                        aTry[nDir + 1 + nFile] = '\0';
                        (void)execve(aTry, aArgv, pEnv);
                    }
                    if (pColon == NULL) {
                        break;
                    }
                    p = pColon + 1;
                }
            }
        } else {
            (void)execve(szPath, aArgv, pEnv);
        }
        _exit(127);
    }
    if (pPid != NULL) {
        *pPid = pid;
    }
    return 0;
}

int
posix_spawn(pid_t *pPid, const char *szPath,
            const posix_spawn_file_actions_t *pFileActions,
            const posix_spawnattr_t *pAttr, char *const aArgv[],
            char *const aEnvp[])
{
    return do_spawn(pPid, szPath, pFileActions, pAttr, aArgv, aEnvp, 0);
}

int
posix_spawnp(pid_t *pPid, const char *szFile,
             const posix_spawn_file_actions_t *pFileActions,
             const posix_spawnattr_t *pAttr, char *const aArgv[],
             char *const aEnvp[])
{
    return do_spawn(pPid, szFile, pFileActions, pAttr, aArgv, aEnvp, 1);
}

int
posix_spawnattr_init(posix_spawnattr_t *pAttr)
{
    if (pAttr == NULL) {
        return EINVAL;
    }
    memset(pAttr, 0, sizeof(*pAttr));
    return 0;
}

int
posix_spawnattr_destroy(posix_spawnattr_t *pAttr)
{
    if (pAttr == NULL) {
        return EINVAL;
    }
    memset(pAttr, 0, sizeof(*pAttr));
    return 0;
}

int
posix_spawnattr_setflags(posix_spawnattr_t *pAttr, short nFlags)
{
    if (pAttr == NULL) {
        return EINVAL;
    }
    pAttr->__flags = nFlags;
    return 0;
}

int
posix_spawnattr_getflags(const posix_spawnattr_t *pAttr, short *pFlags)
{
    if (pAttr == NULL || pFlags == NULL) {
        return EINVAL;
    }
    *pFlags = pAttr->__flags;
    return 0;
}

int
posix_spawnattr_setpgroup(posix_spawnattr_t *pAttr, pid_t nPgroup)
{
    if (pAttr == NULL) {
        return EINVAL;
    }
    pAttr->__pgrp = nPgroup;
    return 0;
}

int
posix_spawnattr_getpgroup(const posix_spawnattr_t *pAttr, pid_t *pPgroup)
{
    if (pAttr == NULL || pPgroup == NULL) {
        return EINVAL;
    }
    *pPgroup = pAttr->__pgrp;
    return 0;
}

int
posix_spawnattr_setsigmask(posix_spawnattr_t *pAttr, const sigset_t *pSigmask)
{
    if (pAttr == NULL || pSigmask == NULL) {
        return EINVAL;
    }
    pAttr->__ss = *pSigmask;
    return 0;
}

int
posix_spawnattr_getsigmask(const posix_spawnattr_t *pAttr, sigset_t *pSigmask)
{
    if (pAttr == NULL || pSigmask == NULL) {
        return EINVAL;
    }
    *pSigmask = pAttr->__ss;
    return 0;
}

int
posix_spawnattr_setsigdefault(posix_spawnattr_t *pAttr,
                              const sigset_t *pSigdefault)
{
    if (pAttr == NULL || pSigdefault == NULL) {
        return EINVAL;
    }
    pAttr->__sd = *pSigdefault;
    return 0;
}

int
posix_spawnattr_getsigdefault(const posix_spawnattr_t *pAttr,
                              sigset_t *pSigdefault)
{
    if (pAttr == NULL || pSigdefault == NULL) {
        return EINVAL;
    }
    *pSigdefault = pAttr->__sd;
    return 0;
}

int
posix_spawnattr_setschedparam(posix_spawnattr_t *pAttr,
                              const struct sched_param *pSchedparam)
{
    if (pAttr == NULL || pSchedparam == NULL) {
        return EINVAL;
    }
    pAttr->__sp = *pSchedparam;
    return 0;
}

int
posix_spawnattr_getschedparam(const posix_spawnattr_t *pAttr,
                              struct sched_param *pSchedparam)
{
    if (pAttr == NULL || pSchedparam == NULL) {
        return EINVAL;
    }
    *pSchedparam = pAttr->__sp;
    return 0;
}

int
posix_spawnattr_setschedpolicy(posix_spawnattr_t *pAttr, int nSchedpolicy)
{
    if (pAttr == NULL) {
        return EINVAL;
    }
    pAttr->__policy = nSchedpolicy;
    return 0;
}

int
posix_spawnattr_getschedpolicy(const posix_spawnattr_t *pAttr,
                               int *pSchedpolicy)
{
    if (pAttr == NULL || pSchedpolicy == NULL) {
        return EINVAL;
    }
    *pSchedpolicy = pAttr->__policy;
    return 0;
}

int
posix_spawn_file_actions_init(posix_spawn_file_actions_t *pFa)
{
    struct gj_spawn_fa *pInner;

    if (pFa == NULL) {
        return EINVAL;
    }
    memset(pFa, 0, sizeof(*pFa));
    pInner = (struct gj_spawn_fa *)calloc(1, sizeof(*pInner));
    if (pInner == NULL) {
        return ENOMEM;
    }
    pFa->__actions = pInner;
    pFa->__used = 0;
    pFa->__allocated = 0;
    return 0;
}

int
posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *pFa)
{
    if (pFa == NULL) {
        return EINVAL;
    }
    fa_free((struct gj_spawn_fa *)pFa->__actions);
    free(pFa->__actions);
    pFa->__actions = NULL;
    memset(pFa, 0, sizeof(*pFa));
    return 0;
}

int
posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *pFa, int nFd)
{
    struct gj_spawn_fa *pInner;

    if (pFa == NULL || pFa->__actions == NULL) {
        return EINVAL;
    }
    pInner = (struct gj_spawn_fa *)pFa->__actions;
    if (fa_grow(pInner) != 0) {
        return errno;
    }
    pInner->pActs[pInner->cUsed].nKind = GJ_SPAWN_ACT_CLOSE;
    pInner->pActs[pInner->cUsed].nFd = nFd;
    pInner->pActs[pInner->cUsed].szPath = NULL;
    pInner->cUsed++;
    pFa->__used = pInner->cUsed;
    return 0;
}

int
posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *pFa, int nFd,
                                 int nNewfd)
{
    struct gj_spawn_fa *pInner;

    if (pFa == NULL || pFa->__actions == NULL) {
        return EINVAL;
    }
    pInner = (struct gj_spawn_fa *)pFa->__actions;
    if (fa_grow(pInner) != 0) {
        return errno;
    }
    pInner->pActs[pInner->cUsed].nKind = GJ_SPAWN_ACT_DUP2;
    pInner->pActs[pInner->cUsed].nFd = nFd;
    pInner->pActs[pInner->cUsed].nNewfd = nNewfd;
    pInner->pActs[pInner->cUsed].szPath = NULL;
    pInner->cUsed++;
    pFa->__used = pInner->cUsed;
    return 0;
}

int
posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *pFa, int nFd,
                                 const char *szPath, int nOflag, mode_t nMode)
{
    struct gj_spawn_fa *pInner;
    char *szDup;

    if (pFa == NULL || pFa->__actions == NULL || szPath == NULL) {
        return EINVAL;
    }
    pInner = (struct gj_spawn_fa *)pFa->__actions;
    if (fa_grow(pInner) != 0) {
        return errno;
    }
    szDup = strdup(szPath);
    if (szDup == NULL) {
        return ENOMEM;
    }
    pInner->pActs[pInner->cUsed].nKind = GJ_SPAWN_ACT_OPEN;
    pInner->pActs[pInner->cUsed].nFd = nFd;
    pInner->pActs[pInner->cUsed].nOflag = nOflag;
    pInner->pActs[pInner->cUsed].nMode = nMode;
    pInner->pActs[pInner->cUsed].szPath = szDup;
    pInner->cUsed++;
    pFa->__used = pInner->cUsed;
    return 0;
}

int
posix_spawn_file_actions_addchdir_np(posix_spawn_file_actions_t *pFa,
                                     const char *szPath)
{
    struct gj_spawn_fa *pInner;
    char *szDup;

    if (pFa == NULL || pFa->__actions == NULL || szPath == NULL) {
        return EINVAL;
    }
    pInner = (struct gj_spawn_fa *)pFa->__actions;
    if (fa_grow(pInner) != 0) {
        return errno;
    }
    szDup = strdup(szPath);
    if (szDup == NULL) {
        return ENOMEM;
    }
    pInner->pActs[pInner->cUsed].nKind = GJ_SPAWN_ACT_CHDIR;
    pInner->pActs[pInner->cUsed].szPath = szDup;
    pInner->cUsed++;
    pFa->__used = pInner->cUsed;
    return 0;
}

int
posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *pFa, int nFd)
{
    struct gj_spawn_fa *pInner;

    if (pFa == NULL || pFa->__actions == NULL) {
        return EINVAL;
    }
    pInner = (struct gj_spawn_fa *)pFa->__actions;
    if (fa_grow(pInner) != 0) {
        return errno;
    }
    pInner->pActs[pInner->cUsed].nKind = GJ_SPAWN_ACT_FCHDIR;
    pInner->pActs[pInner->cUsed].nFd = nFd;
    pInner->pActs[pInner->cUsed].szPath = NULL;
    pInner->cUsed++;
    pFa->__used = pInner->cUsed;
    return 0;
}

int
posix_spawn_file_actions_addclosefrom_np(posix_spawn_file_actions_t *pFa,
                                         int nFrom)
{
    struct gj_spawn_fa *pInner;

    if (pFa == NULL || pFa->__actions == NULL) {
        return EINVAL;
    }
    pInner = (struct gj_spawn_fa *)pFa->__actions;
    if (fa_grow(pInner) != 0) {
        return errno;
    }
    pInner->pActs[pInner->cUsed].nKind = GJ_SPAWN_ACT_CLOSEFROM;
    pInner->pActs[pInner->cUsed].nFd = nFrom;
    pInner->pActs[pInner->cUsed].szPath = NULL;
    pInner->cUsed++;
    pFa->__used = pInner->cUsed;
    return 0;
}
