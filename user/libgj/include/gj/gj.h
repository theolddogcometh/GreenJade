/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Umbrella header for freestanding libgj (string helpers + syscall wrappers).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0 (project root LICENSE; SPDX above).
 * Not a libc: no malloc, no stdio, no TLS, no locale. Product path for
 * freestanding GreenJade user ELFs (init, shell, ld-gj, door servers).
 *
 * Include this single header for the full public surface, or pull the parts:
 *
 *   #include <gj/gj.h>          — string + syscalls (this file)
 *   #include <gj/string.h>      — length/compare/copy/search/case/class/memory/
 *                                 parse/format + thin Linux write/read / dlog
 *   #include <gj/syscalls.h>    — GJ_SYS_* / door ops / LINUX_NR_* + inlines;
 *                                 declares gj_syscall6 (implemented in
 *                                 user/libgj/src/syscall.S, archived in libgj.a)
 *
 * Build notes (parent Makefile owns LIBGJ_OBJS):
 *   -Iuser/libgj/include  + link build/user/libgj.a
 *   Typical freestanding flags: -std=c11 -ffreestanding -fno-builtin
 *   -fno-stack-protector -fno-pic -mno-red-zone (x86_64) -Wall -Wextra -Werror
 *
 * Ownership boundary:
 *   - This tree is freestanding only. Do not merge with user/libcgj/ (glibc
 *     surface) or sibling product SOs (user/libgj-so/, user/libgj-gnu/).
 *   - Native GJ_SYS_* numbers must stay aligned with kernel/include/gj/syscall.h
 *     (sparse blocks; do not renumber after userland ships).
 *   - Door op macros track kernel door headers (session/net/store/vfs/…).
 *
 * Soft deepen: documentation and thin header wrappers only — no policy in
 * libgj; the kernel (or door server) enforces capability / door claim rules.
 */
#pragma once

#include <gj/string.h>
#include <gj/syscalls.h>
