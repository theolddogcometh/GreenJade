/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7846: regexec REG_STARTEND presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_has_startend_u_7846(uint32_t eflags);
 *     - Return 1 if eflags has REG_STARTEND (0x4) set, else 0.
 *   uint32_t __gj_regexec_has_startend_u_7846  (alias)
 *   __libcgj_batch7846_marker = "libcgj-batch7846"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_has_startend_u_7846 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7846_marker[] = "libcgj-batch7846";

/* REG_STARTEND bit (0x4). */
#define B7846_REG_STARTEND ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7846_regexec_has_startend(uint32_t u32Eflags)
{
	return ((u32Eflags & B7846_REG_STARTEND) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_has_startend_u_7846 - 1 if eflags includes REG_STARTEND.
 *
 * eflags: regexec(3) eflags argument
 *
 * Returns 1 when bit 0x4 is set, else 0. Soft bit test only; does not
 * call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_has_startend_u_7846(uint32_t u32Eflags)
{
	(void)NULL;
	return b7846_regexec_has_startend(u32Eflags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_has_startend_u_7846(uint32_t u32Eflags)
    __attribute__((alias("gj_regexec_has_startend_u_7846")));
