/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/wait.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * wait/waitpid/wait3/wait4, W* status macros, and idtype waitid shapes used
 * after fork/posix_spawn.
 *
 * Design notes
 * ------------
 * Status packing macros match Linux. Hybrid process model maps to GreenJade
 * tasks/doors under the Linux personality.
 *
 * Non-goals
 * ---------
 * Every historical rusage field accuracy on soft paths.
 */
#pragma once

#include <signal.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WNOHANG    1
#define WUNTRACED  2
#define WCONTINUED 8
#define WNOWAIT    0x1000000
#define WEXITED    4
#define WSTOPPED   2
#define WNOHANG_W  WNOHANG

#define WEXITSTATUS(s)  (((s) >> 8) & 0xff)
#define WTERMSIG(s)     ((s) & 0x7f)
#define WIFEXITED(s)    (WTERMSIG(s) == 0)
#define WIFSIGNALED(s)  (((s) & 0x7f) != 0 && ((s) & 0x80) == 0)

typedef enum {
    P_ALL  = 0,
    P_PID  = 1,
    P_PGID = 2,
    P_PIDFD = 3
} idtype_t;

struct rusage;

pid_t wait(int *pStatus);
pid_t waitpid(pid_t pid, int *pStatus, int nOptions);
pid_t wait3(int *pStatus, int nOptions, struct rusage *pRusage);
pid_t wait4(pid_t pid, int *pStatus, int nOptions, struct rusage *pRusage);
int   waitid(idtype_t idtype, id_t id, siginfo_t *pInfop, int nOptions);

#ifdef __cplusplus
}
#endif

