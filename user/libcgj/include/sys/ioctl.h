/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ioctl (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/ioctl.h>. Constants and type sizes
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
 * Soft residual lean (this unit; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   - ioctl vararg surface; soft fills may ENOSYS until hybrid ABI
 *   - Soft!=product: libcgj sys/ioctl surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj sys/ioctl.h: soft residual lean Soft!=product
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Linux x86 tty/socket ioctls OpenSSH sshpty/channels use. */
#ifndef TIOCSCTTY
#define TIOCSCTTY  0x540E
#endif
#ifndef TIOCGPGRP
#define TIOCGPGRP  0x540F
#endif
#ifndef TIOCSPGRP
#define TIOCSPGRP  0x5410
#endif
#ifndef TIOCGWINSZ
#define TIOCGWINSZ 0x5413
#endif
#ifndef TIOCSWINSZ
#define TIOCSWINSZ 0x5414
#endif
#ifndef TIOCNOTTY
#define TIOCNOTTY  0x5422
#endif
#ifndef TIOCSBRK
#define TIOCSBRK   0x5427
#endif
#ifndef TIOCCBRK
#define TIOCCBRK   0x5428
#endif
#ifndef TCGETS
#define TCGETS  0x5401
#endif
#ifndef TCSETS
#define TCSETS  0x5402
#endif
#ifndef TCSETSW
#define TCSETSW 0x5403
#endif
#ifndef TCSETSF
#define TCSETSF 0x5404
#endif
#ifndef TCSBRK
#define TCSBRK  0x5409
#endif
#ifndef TCXONC
#define TCXONC  0x540A
#endif
#ifndef TCFLSH
#define TCFLSH  0x540B
#endif
#ifndef TIOCGSID
#define TIOCGSID   0x5429
#endif
#ifndef TIOCGPTN
#define TIOCGPTN   0x80045430u
#endif
#ifndef TIOCSPTLCK
#define TIOCSPTLCK 0x40045431u
#endif
#ifndef TIOCGPTPEER
#define TIOCGPTPEER 0x5441 /* Linux _IO('T', 0x41) */
#endif
#ifndef FIONREAD
#define FIONREAD   0x541B
#endif
#ifndef FIONBIO
#define FIONBIO    0x5421
#endif

#ifndef _GJ_OPENSSH_WINSIZE
#define _GJ_OPENSSH_WINSIZE 1
#endif
#ifndef _GJ_WINSIZE_DEFINED
#define _GJ_WINSIZE_DEFINED
struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};
#endif

int ioctl(int nFd, unsigned long u64Req, ...);

#ifdef __cplusplus
}
#endif
