/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7839: regex cflags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_cflags_pack_u_7839(uint32_t want_extended,
 *                                      uint32_t want_icase,
 *                                      uint32_t want_newline,
 *                                      uint32_t want_nosub);
 *     - Pack soft REG_* cflag wants into a mask (0x1|0x2|0x4|0x8).
 *   uint32_t __gj_reg_cflags_pack_u_7839  (alias)
 *   __libcgj_batch7839_marker = "libcgj-batch7839"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_cflags_pack_u_7839 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7839_marker[] = "libcgj-batch7839";

/* Soft pack bits matching POSIX regcomp cflags layout. */
#define B7839_REG_EXTENDED ((uint32_t)0x1u)
#define B7839_REG_ICASE    ((uint32_t)0x2u)
#define B7839_REG_NEWLINE  ((uint32_t)0x4u)
#define B7839_REG_NOSUB    ((uint32_t)0x8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7839_reg_cflags_pack(uint32_t u32WantExtended,
                      uint32_t u32WantIcase,
                      uint32_t u32WantNewline,
                      uint32_t u32WantNosub)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantExtended != 0u)
		u32Pack |= B7839_REG_EXTENDED;
	if (u32WantIcase != 0u)
		u32Pack |= B7839_REG_ICASE;
	if (u32WantNewline != 0u)
		u32Pack |= B7839_REG_NEWLINE;
	if (u32WantNosub != 0u)
		u32Pack |= B7839_REG_NOSUB;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_cflags_pack_u_7839 - soft-pack regcomp cflag wants.
 *
 * want_extended: non-zero to set REG_EXTENDED (0x1)
 * want_icase:    non-zero to set REG_ICASE (0x2)
 * want_newline:  non-zero to set REG_NEWLINE (0x4)
 * want_nosub:    non-zero to set REG_NOSUB (0x8)
 *
 * Returns bitmask of requested REG_* cflags. Pure integer pack; does
 * not call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_cflags_pack_u_7839(uint32_t u32WantExtended,
                          uint32_t u32WantIcase,
                          uint32_t u32WantNewline,
                          uint32_t u32WantNosub)
{
	(void)NULL;
	return b7839_reg_cflags_pack(u32WantExtended, u32WantIcase,
	                             u32WantNewline, u32WantNosub);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_cflags_pack_u_7839(uint32_t u32WantExtended,
                                     uint32_t u32WantIcase,
                                     uint32_t u32WantNewline,
                                     uint32_t u32WantNosub)
    __attribute__((alias("gj_reg_cflags_pack_u_7839")));
