/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7734: fchdir AT_FDCWD soft u32 tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_at_fdcwd_u_7734(void);
 *     - Returns soft AT_FDCWD as uint32_t two's-complement tag
 *       ((uint32_t)-100 == 0xffffff9c). Catalog only; not a live fd.
 *   uint32_t __gj_fchdir_at_fdcwd_u_7734  (alias)
 *   __libcgj_batch7734_marker = "libcgj-batch7734"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_at_fdcwd_u_7734 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7734_marker[] = "libcgj-batch7734";

/*
 * Soft AT_FDCWD (-100) as uint32_t bit pattern. Matches Linux/glibc
 * AT_FDCWD without fcntl.h. Not a valid non-negative fchdir fd.
 */
#define B7734_AT_FDCWD_U  ((uint32_t)(-100))

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7734_at_fdcwd_u(void)
{
	return B7734_AT_FDCWD_U;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_at_fdcwd_u_7734 - soft AT_FDCWD catalog tag as uint32_t.
 *
 * Always returns (uint32_t)-100 (0xffffff9c). Distinct from non-negative
 * fchdir fd gates; pure-data layout tag only. Does not call fchdir.
 * No parent wires.
 */
uint32_t
gj_fchdir_at_fdcwd_u_7734(void)
{
	(void)NULL;
	return b7734_at_fdcwd_u();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_at_fdcwd_u_7734(void)
    __attribute__((alias("gj_fchdir_at_fdcwd_u_7734")));
