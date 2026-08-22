/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <sys/ptrace.h> for libcgj (HAVE_SYS_PTRACE_H).
 * OpenSSH platform-tracing on Linux uses PR_SET_DUMPABLE, not ptrace.
 * Do not define PT_DENY_ATTACH (Darwin). Dual DoD B OPEN.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PTRACE_TRACEME
#define PTRACE_TRACEME    0
#define PTRACE_PEEKTEXT   1
#define PTRACE_PEEKDATA   2
#define PTRACE_PEEKUSER   3
#define PTRACE_POKETEXT   4
#define PTRACE_POKEDATA   5
#define PTRACE_POKEUSER   6
#define PTRACE_CONT       7
#define PTRACE_KILL       8
#define PTRACE_SINGLESTEP 9
#define PTRACE_GETREGS    12
#define PTRACE_SETREGS    13
#define PTRACE_ATTACH     16
#define PTRACE_DETACH     17
#define PTRACE_SYSCALL    24
#define PTRACE_SETOPTIONS 0x4200
#define PTRACE_GETEVENTMSG 0x4201
#define PTRACE_SEIZE      0x4206
#endif

#ifndef PTRACE_O_TRACESYSGOOD
#define PTRACE_O_TRACESYSGOOD 1
#endif

long ptrace(int nRequest, ...);

#ifdef __cplusplus
}
#endif
