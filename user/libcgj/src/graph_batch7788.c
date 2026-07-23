/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7788: glob flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_flags_pack_u_7788(uint32_t want_err,
 *                                      uint32_t want_mark,
 *                                      uint32_t want_nocheck);
 *     - Pack soft GLOB_* wants into a flags mask (0x1|0x2|0x10).
 *   uint32_t __gj_glob_flags_pack_u_7788  (alias)
 *   __libcgj_batch7788_marker = "libcgj-batch7788"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_flags_pack_u_7788 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7788_marker[] = "libcgj-batch7788";

/* Soft pack bits matching POSIX/glibc GLOB_* layout. */
#define B7788_GLOB_ERR     ((uint32_t)0x1u)
#define B7788_GLOB_MARK    ((uint32_t)0x2u)
#define B7788_GLOB_NOCHECK ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7788_glob_flags_pack(uint32_t u32WantErr,
                      uint32_t u32WantMark,
                      uint32_t u32WantNocheck)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantErr != 0u)
		u32Pack |= B7788_GLOB_ERR;
	if (u32WantMark != 0u)
		u32Pack |= B7788_GLOB_MARK;
	if (u32WantNocheck != 0u)
		u32Pack |= B7788_GLOB_NOCHECK;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_flags_pack_u_7788 - soft-pack glob flag wants.
 *
 * want_err:     non-zero to set GLOB_ERR (0x1)
 * want_mark:    non-zero to set GLOB_MARK (0x2)
 * want_nocheck: non-zero to set GLOB_NOCHECK (0x10)
 *
 * Returns bitmask of requested GLOB_* bits. Pure integer pack; does not
 * call glob(3). No parent wires.
 */
uint32_t
gj_glob_flags_pack_u_7788(uint32_t u32WantErr,
                          uint32_t u32WantMark,
                          uint32_t u32WantNocheck)
{
	(void)NULL;
	return b7788_glob_flags_pack(u32WantErr, u32WantMark,
	                             u32WantNocheck);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_flags_pack_u_7788(uint32_t u32WantErr,
                                     uint32_t u32WantMark,
                                     uint32_t u32WantNocheck)
    __attribute__((alias("gj_glob_flags_pack_u_7788")));
