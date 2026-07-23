/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7847: regexec eflags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_eflags_ok_u_7847(uint32_t eflags);
 *     - Return 1 if eflags uses only known REG_* eflag bits
 *       (0x1|0x2|0x4), else 0.
 *   uint32_t __gj_regexec_eflags_ok_u_7847  (alias)
 *   __libcgj_batch7847_marker = "libcgj-batch7847"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_eflags_ok_u_7847 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7847_marker[] = "libcgj-batch7847";

/*
 * Known regexec eflag bits for this continuum:
 *   REG_NOTBOL (0x1) | REG_NOTEOL (0x2) | REG_STARTEND (0x4) = 0x7
 */
#define B7847_REG_EFLAGS_KNOWN_MASK ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7847_regexec_eflags_ok(uint32_t u32Eflags)
{
	return ((u32Eflags & ~B7847_REG_EFLAGS_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_eflags_ok_u_7847 - 1 if eflags has no unknown bits.
 *
 * eflags: regexec(3) eflags argument
 *
 * Returns 1 when only bits in {0x1, 0x2, 0x4} are set (including 0),
 * else 0. Soft catalog check; does not call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_eflags_ok_u_7847(uint32_t u32Eflags)
{
	(void)NULL;
	return b7847_regexec_eflags_ok(u32Eflags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_eflags_ok_u_7847(uint32_t u32Eflags)
    __attribute__((alias("gj_regexec_eflags_ok_u_7847")));
