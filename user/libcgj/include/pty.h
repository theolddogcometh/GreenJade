/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped pty.h (stubs). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <pty.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj pty.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = Unix98 /dev/ptmx + TIOCSPTLCK; socketpair fallback
 *   product  = session/term path not Dual DoD B close; not .ko AC
 *   honesty  = pty surface != interactive SSH login; Dual DoD B OPEN
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: pty soft residual Soft!=product
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* termios/winsize optional; pass NULL or opaque until termios lands */
struct termios;
struct winsize;

int openpty(int *pAmaster, int *pAslave, char *szName,
            const struct termios *pTermp, const struct winsize *pWinp);
int forkpty(int *pAmaster, char *szName, const struct termios *pTermp,
            const struct winsize *pWinp);
int login_tty(int nFd);
int posix_openpt(int nFlags);
int grantpt(int nFd);
int unlockpt(int nFd);
char *ptsname(int nFd);
int  ptsname_r(int nFd, char *szBuf, size_t cb);
int  getpt(void);

#ifdef __cplusplus
}
#endif
