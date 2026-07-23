/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5059: test the sticky bit (S_ISVTX) in mode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_sticky_u(uint32_t mode);
 *     - 1 if the sticky bit (octal 01000 / 0x200) is set, else 0.
 *   uint32_t __gj_fs_sticky_u  (alias)
 *   __libcgj_batch5059_marker = "libcgj-batch5059"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from perm/type
 * extractors — unique gj_fs_sticky_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5059_marker[] = "libcgj-batch5059";

/* POSIX S_ISVTX: sticky bit (octal 01000). */
#define B5059_S_ISVTX  ((uint32_t)01000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5059_sticky(uint32_t u32Mode)
{
	if ((u32Mode & B5059_S_ISVTX) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_sticky_u - 1 if the sticky bit is set in mode, else 0.
 *
 * mode: POSIX-style st_mode bitfield
 *
 * Returns 1 when bit 0x200 (octal 01000, S_ISVTX) is set, else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fs_sticky_u(uint32_t u32Mode)
{
	(void)NULL;
	return b5059_sticky(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_sticky_u(uint32_t u32Mode)
    __attribute__((alias("gj_fs_sticky_u")));
