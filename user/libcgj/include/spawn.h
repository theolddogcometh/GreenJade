/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <spawn.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * posix_spawn/posix_spawnp with attr and file_actions (fork+exec shaped
 * bring-up). Preferred over raw fork/exec for thread-heavy parents.
 *
 * Design notes
 * ------------
 * File actions cover open/close/dup2 sequences applied in the child before
 * exec. Soft paths may not implement every POSIX_SPAWN_* flag.
 *
 * Non-goals
 * ---------
 * vfork-only spawn without a working process model.
 */
#pragma once

#include <sched.h>
#include <signal.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POSIX_SPAWN_RESETIDS      0x01
#define POSIX_SPAWN_SETPGROUP     0x02
#define POSIX_SPAWN_SETSIGDEF     0x04
#define POSIX_SPAWN_SETSIGMASK    0x08
#define POSIX_SPAWN_SETSCHEDPARAM 0x10
#define POSIX_SPAWN_SETSCHEDULER  0x20
#define POSIX_SPAWN_USEVFORK      0x40
#define POSIX_SPAWN_SETSID        0x80

typedef struct {
    short          __flags;
    short          __pad0;
    pid_t          __pgrp;
    sigset_t       __sd;
    sigset_t       __ss;
    struct sched_param __sp;
    int            __policy;
    int            __pad[16];
} posix_spawnattr_t;

typedef struct {
    int   __used;
    int   __allocated;
    void *__actions; /* internal action list */
    int   __pad[16];
} posix_spawn_file_actions_t;

int posix_spawn(pid_t *pPid, const char *szPath,
                const posix_spawn_file_actions_t *pFileActions,
                const posix_spawnattr_t *pAttr, char *const aArgv[],
                char *const aEnvp[]);
int posix_spawnp(pid_t *pPid, const char *szFile,
                 const posix_spawn_file_actions_t *pFileActions,
                 const posix_spawnattr_t *pAttr, char *const aArgv[],
                 char *const aEnvp[]);

int posix_spawnattr_init(posix_spawnattr_t *pAttr);
int posix_spawnattr_destroy(posix_spawnattr_t *pAttr);
int posix_spawnattr_setflags(posix_spawnattr_t *pAttr, short nFlags);
int posix_spawnattr_getflags(const posix_spawnattr_t *pAttr, short *pFlags);
int posix_spawnattr_setpgroup(posix_spawnattr_t *pAttr, pid_t nPgroup);
int posix_spawnattr_getpgroup(const posix_spawnattr_t *pAttr, pid_t *pPgroup);
int posix_spawnattr_setsigmask(posix_spawnattr_t *pAttr,
                               const sigset_t *pSigmask);
int posix_spawnattr_getsigmask(const posix_spawnattr_t *pAttr,
                               sigset_t *pSigmask);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *pAttr,
                                  const sigset_t *pSigdefault);
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *pAttr,
                                  sigset_t *pSigdefault);
int posix_spawnattr_setschedparam(posix_spawnattr_t *pAttr,
                                  const struct sched_param *pSchedparam);
int posix_spawnattr_getschedparam(const posix_spawnattr_t *pAttr,
                                  struct sched_param *pSchedparam);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *pAttr, int nSchedpolicy);
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *pAttr,
                                   int *pSchedpolicy);

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *pFa);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *pFa);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *pFa, int nFd,
                                     const char *szPath, int nOflag,
                                     mode_t nMode);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *pFa, int nFd);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *pFa, int nFd,
                                     int nNewfd);
int posix_spawn_file_actions_addchdir_np(posix_spawn_file_actions_t *pFa,
                                         const char *szPath);
int posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *pFa,
                                          int nFd);
int posix_spawn_file_actions_addclosefrom_np(posix_spawn_file_actions_t *pFa,
                                             int nFrom);
int posix_spawn_file_actions_addtcsetpgrp_np(posix_spawn_file_actions_t *pFa,
                                             int nFd);

#ifdef __cplusplus
}
#endif
