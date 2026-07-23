/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7835: regex REG_EXTENDED presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_has_extended_u_7835(uint32_t cflags);
 *     - Return 1 if cflags has REG_EXTENDED (0x1) set, else 0.
 *   uint32_t __gj_reg_has_extended_u_7835  (alias)
 *   __libcgj_batch7835_marker = "libcgj-batch7835"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_has_extended_u_7835 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7835_marker[] = "libcgj-batch7835";

/* REG_EXTENDED bit (0x1). */
#define B7835_REG_EXTENDED ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7835_reg_has_extended(uint32_t u32Cflags)
{
	return ((u32Cflags & B7835_REG_EXTENDED) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_has_extended_u_7835 - 1 if cflags includes REG_EXTENDED.
 *
 * cflags: regcomp(3) cflags argument
 *
 * Returns 1 when bit 0x1 is set, else 0. Soft bit test only; does not
 * call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_has_extended_u_7835(uint32_t u32Cflags)
{
	(void)NULL;
	return b7835_reg_has_extended(u32Cflags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_has_extended_u_7835(uint32_t u32Cflags)
    __attribute__((alias("gj_reg_has_extended_u_7835")));
