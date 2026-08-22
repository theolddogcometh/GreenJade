/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <linux/seccomp.h> for libcgj. OpenSSH sandbox-seccomp-filter
 * (HAVE_LINUX_SECCOMP_H) needs SECCOMP_MODE_FILTER, SECCOMP_RET_* and
 * struct seccomp_data. ABI numbers match Linux x86_64 UAPI.
 * Dual DoD B OPEN. SANDBOX_NULL remains the glue sandbox.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SECCOMP_MODE_DISABLED
#define SECCOMP_MODE_DISABLED 0
#endif
#ifndef SECCOMP_MODE_STRICT
#define SECCOMP_MODE_STRICT 1
#endif
#ifndef SECCOMP_MODE_FILTER
#define SECCOMP_MODE_FILTER 2
#endif

#ifndef SECCOMP_RET_KILL_PROCESS
#define SECCOMP_RET_KILL_PROCESS 0x80000000u
#endif
#ifndef SECCOMP_RET_KILL_THREAD
#define SECCOMP_RET_KILL_THREAD  0x00000000u
#endif
#ifndef SECCOMP_RET_KILL
#define SECCOMP_RET_KILL  0x00000000u
#endif
#ifndef SECCOMP_SET_MODE_STRICT
#define SECCOMP_SET_MODE_STRICT 0
#endif
#ifndef SECCOMP_SET_MODE_FILTER
#define SECCOMP_SET_MODE_FILTER 1
#endif
#ifndef SECCOMP_RET_TRAP
#define SECCOMP_RET_TRAP  0x00030000u
#endif
#ifndef SECCOMP_RET_ERRNO
#define SECCOMP_RET_ERRNO 0x00050000u
#endif
#ifndef SECCOMP_RET_TRACE
#define SECCOMP_RET_TRACE 0x00070000u
#endif
#ifndef SECCOMP_RET_ALLOW
#define SECCOMP_RET_ALLOW 0x7fff0000u
#endif

#ifndef SECCOMP_RET_ACTION
#define SECCOMP_RET_ACTION 0x7fff0000u
#define SECCOMP_RET_DATA   0x0000ffffu
#endif

struct seccomp_data {
    int      nr;
    uint32_t arch;
    uint64_t instruction_pointer;
    uint64_t args[6];
};

#ifdef __cplusplus
}
#endif
