/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7735: fchdir x86_64 syscall number.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_nr_x86_64_7735(void);
 *     - Returns 81 (soft Linux x86_64 NR_fchdir catalog tag).
 *   uint32_t __gj_fchdir_nr_x86_64_7735  (alias)
 *   __libcgj_batch7735_marker = "libcgj-batch7735"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique gj_fchdir_nr_x86_64_7735 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7735_marker[] = "libcgj-batch7735";

/* Soft Linux x86_64 NR_fchdir (sys_fchdir). Catalog only. */
#define B7735_NR_FCHDIR_X86_64  81u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7735_nr_x86_64(void)
{
	return B7735_NR_FCHDIR_X86_64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_nr_x86_64_7735 - soft Linux x86_64 NR_fchdir tag.
 *
 * Always returns 81. Catalog id only; does not issue a syscall.
 * No parent wires.
 */
uint32_t
gj_fchdir_nr_x86_64_7735(void)
{
	(void)NULL;
	return b7735_nr_x86_64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fchdir_nr_x86_64_7735(void)
    __attribute__((alias("gj_fchdir_nr_x86_64_7735")));
