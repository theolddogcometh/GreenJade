/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1878: Unix mode permission / special bits.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_perms_octal(uint32_t mode);
 *     — Return the low 12 bits of a mode_t-like u32: permission bits
 *       plus setuid/setgid/sticky. Mask is the standard Unix 07777
 *       (0xFFF) which covers:
 *         S_ISUID 0004000, S_ISGID 0002000, S_ISVTX 0001000
 *         S_IRWXU 00700,   S_IRWXG 00070,   S_IRWXO 00007
 *       File-type bits (S_IFMT 0170000) are cleared. Pure-data extract.
 *   uint32_t __gj_mode_perms_octal  (alias)
 *   __libcgj_batch1878_marker = "libcgj-batch1878"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit helpers.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1878_marker[] = "libcgj-batch1878";

/* Low 12 bits: setuid/setgid/sticky + rwx for u/g/o (07777). */
#define B1878_PERMS  07777u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1878_perms(uint32_t uMode)
{
	return uMode & B1878_PERMS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_perms_octal — low 12 permission/special bits of mode.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns mode & 07777 (0xFFF); type bits above bit 11 are zeroed.
 */
uint32_t
gj_mode_perms_octal(uint32_t uMode)
{
	return b1878_perms(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_perms_octal(uint32_t uMode)
    __attribute__((alias("gj_mode_perms_octal")));
