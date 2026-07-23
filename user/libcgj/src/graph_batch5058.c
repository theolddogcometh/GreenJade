/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5058: extract file-type nibble from mode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_type_u(uint32_t mode);
 *     - Return mode & 0170000 (S_IFMT, 0xf000).
 *   uint32_t __gj_fs_type_u  (alias)
 *   __libcgj_batch5058_marker = "libcgj-batch5058"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from
 * gj_fs_perm_u (batch5057) and is_* predicates — unique gj_fs_type_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5058_marker[] = "libcgj-batch5058";

/* POSIX S_IFMT: file type mask (octal 0170000). */
#define B5058_S_IFMT  ((uint32_t)0170000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5058_type(uint32_t u32Mode)
{
	return u32Mode & B5058_S_IFMT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_type_u - extract the file-type bits from st_mode.
 *
 * mode: POSIX-style st_mode bitfield
 *
 * Returns mode & 0170000 (0xf000). Compare against S_IFREG/S_IFDIR/etc.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fs_type_u(uint32_t u32Mode)
{
	(void)NULL;
	return b5058_type(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_type_u(uint32_t u32Mode)
    __attribute__((alias("gj_fs_type_u")));
