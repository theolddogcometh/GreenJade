/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7836: regex REG_ICASE presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_reg_has_icase_u_7836(uint32_t cflags);
 *     - Return 1 if cflags has REG_ICASE (0x2) set, else 0.
 *   uint32_t __gj_reg_has_icase_u_7836  (alias)
 *   __libcgj_batch7836_marker = "libcgj-batch7836"
 *
 * Exclusive continuum CREATE-ONLY (7831-7840: regex cflags stubs —
 * extended_id, icase_id, newline_id, nosub_id, has_extended, has_icase,
 * has_newline, has_nosub, cflags_pack, batch_id_7840).
 * Unique gj_reg_has_icase_u_7836 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7836_marker[] = "libcgj-batch7836";

/* REG_ICASE bit (0x2). */
#define B7836_REG_ICASE ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7836_reg_has_icase(uint32_t u32Cflags)
{
	return ((u32Cflags & B7836_REG_ICASE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reg_has_icase_u_7836 - 1 if cflags includes REG_ICASE.
 *
 * cflags: regcomp(3) cflags argument
 *
 * Returns 1 when bit 0x2 is set, else 0. Soft bit test only; does not
 * call regcomp(3). No parent wires.
 */
uint32_t
gj_reg_has_icase_u_7836(uint32_t u32Cflags)
{
	(void)NULL;
	return b7836_reg_has_icase(u32Cflags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_reg_has_icase_u_7836(uint32_t u32Cflags)
    __attribute__((alias("gj_reg_has_icase_u_7836")));
