/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5057: extract low 9 permission bits from mode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_perm_u(uint32_t mode);
 *     - Return mode & 0777 (rwxrwxrwx mask, 0x1ff).
 *   uint32_t __gj_fs_perm_u  (alias)
 *   __libcgj_batch5057_marker = "libcgj-batch5057"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from type / sticky
 * extractors in this wave — unique gj_fs_perm_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5057_marker[] = "libcgj-batch5057";

/* POSIX permission bits: rwxrwxrwx (octal 0777). */
#define B5057_PERM_MASK  ((uint32_t)0777u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5057_perm(uint32_t u32Mode)
{
	return u32Mode & B5057_PERM_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_perm_u - extract owner/group/other rwx permission bits.
 *
 * mode: POSIX-style st_mode bitfield
 *
 * Returns mode & 0777 (0..511). Self-contained; no parent wires.
 */
uint32_t
gj_fs_perm_u(uint32_t u32Mode)
{
	(void)NULL;
	return b5057_perm(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_perm_u(uint32_t u32Mode)
    __attribute__((alias("gj_fs_perm_u")));
