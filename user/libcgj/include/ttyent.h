/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ttyent.h. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <ttyent.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 *
 * Soft residual (C2 libcgj header; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft path  = getttyent/getttynam/setttyent soft fill (often empty table)
 *   product    = full /etc/ttys product registry (OPEN; not closed here)
 *   catalog    = struct ttyent + TTY_ON/TTY_SECURE + get*/set*/end*/ttyslot
 *   honesty    = Soft!=product; soft empty table != product multi-tty mint
 *   Bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION from this unit.
 * greppable: libcgj: soft residual ttyent
 * greppable: libcgj: soft residual ttyent Soft!=product
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ttyent {
    char *ty_name;
    char *ty_getty;
    char *ty_type;
    int   ty_status;
    char *ty_window;
    char *ty_comment;
};

#define TTY_ON     0x01
#define TTY_SECURE 0x02

struct ttyent *getttyent(void);
struct ttyent *getttynam(const char *szName);
int            setttyent(void);
int            endttyent(void);
int            ttyslot(void);

#ifdef __cplusplus
}
#endif
