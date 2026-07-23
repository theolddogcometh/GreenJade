/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7849: regexec eflags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_eflags_errorish_u_7849(uint32_t eflags);
 *     - Return 1 if eflags has any unknown bits outside known REG_*
 *       eflag mask (0x7), else 0.
 *   uint32_t __gj_regexec_eflags_errorish_u_7849  (alias)
 *   __libcgj_batch7849_marker = "libcgj-batch7849"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_eflags_errorish_u_7849 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7849_marker[] = "libcgj-batch7849";

/*
 * Known regexec eflag bits for this continuum:
 *   REG_NOTBOL | REG_NOTEOL | REG_STARTEND = 0x7
 */
#define B7849_REG_EFLAGS_KNOWN_MASK ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7849_regexec_eflags_errorish(uint32_t u32Eflags)
{
	return ((u32Eflags & ~B7849_REG_EFLAGS_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_eflags_errorish_u_7849 - 1 if eflags has unknown bits.
 *
 * eflags: regexec(3) eflags argument
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x4} is set, else 0.
 * Soft inverse of eflags_ok; does not call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_eflags_errorish_u_7849(uint32_t u32Eflags)
{
	(void)NULL;
	return b7849_regexec_eflags_errorish(u32Eflags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_eflags_errorish_u_7849(uint32_t u32Eflags)
    __attribute__((alias("gj_regexec_eflags_errorish_u_7849")));
