/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1879: Unix mode set-user-ID bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_mode_setuid_p(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has the S_ISUID bit set, else 0.
 *       Uses the standard Unix numeric constant (no sys/stat.h):
 *         S_ISUID = 0004000 (0x800)  set-user-ID on execution
 *       Does not inspect file-type or other permission bits.
 *   int __gj_mode_setuid_p  (alias)
 *   __libcgj_batch1879_marker = "libcgj-batch1879"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1879_marker[] = "libcgj-batch1879";

/* Unix set-user-ID bit (octal 0004000). */
#define B1879_ISUID  0004000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1879_setuid_p(uint32_t uMode)
{
	return ((uMode & B1879_ISUID) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_setuid_p — 1 if mode has S_ISUID set.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0004000) != 0, else 0.
 */
int
gj_mode_setuid_p(uint32_t uMode)
{
	return b1879_setuid_p(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_setuid_p(uint32_t uMode)
    __attribute__((alias("gj_mode_setuid_p")));
