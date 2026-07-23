/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5054: test st_mode for regular-file type.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_is_reg_u(uint32_t mode);
 *     - 1 if (mode & S_IFMT) == S_IFREG, else 0.
 *   uint32_t __gj_fs_is_reg_u  (alias)
 *   __libcgj_batch5054_marker = "libcgj-batch5054"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from
 * gj_fs_mode_reg_u (batch5051) — unique gj_fs_is_reg_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5054_marker[] = "libcgj-batch5054";

/* POSIX S_IFMT / S_IFREG without libc sys/stat.h. */
#define B5054_S_IFMT   ((uint32_t)0xf000u)  /* 0170000 */
#define B5054_S_IFREG  ((uint32_t)0x8000u)  /* 0100000 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5054_is_reg(uint32_t u32Mode)
{
	if ((u32Mode & B5054_S_IFMT) == B5054_S_IFREG) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_is_reg_u - 1 if mode encodes a regular file, else 0.
 *
 * mode: POSIX-style st_mode bitfield
 *
 * Returns 1 when the type nibble equals S_IFREG (0x8000), else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fs_is_reg_u(uint32_t u32Mode)
{
	(void)NULL;
	return b5054_is_reg(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_is_reg_u(uint32_t u32Mode)
    __attribute__((alias("gj_fs_is_reg_u")));
