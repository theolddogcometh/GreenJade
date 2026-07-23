/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7844: regexec REG_NOTBOL presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_regexec_has_notbol_u_7844(uint32_t eflags);
 *     - Return 1 if eflags has REG_NOTBOL (0x1) set, else 0.
 *   uint32_t __gj_regexec_has_notbol_u_7844  (alias)
 *   __libcgj_batch7844_marker = "libcgj-batch7844"
 *
 * Exclusive continuum CREATE-ONLY (7841-7850: regexec eflags stubs —
 * notbol_id, noteol_id, startend_id, has_notbol, has_noteol,
 * has_startend, eflags_ok, eflags_pack, eflags_errorish, batch_id_7850).
 * Unique gj_regexec_has_notbol_u_7844 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7844_marker[] = "libcgj-batch7844";

/* REG_NOTBOL bit (0x1). */
#define B7844_REG_NOTBOL ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7844_regexec_has_notbol(uint32_t u32Eflags)
{
	return ((u32Eflags & B7844_REG_NOTBOL) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regexec_has_notbol_u_7844 - 1 if eflags includes REG_NOTBOL.
 *
 * eflags: regexec(3) eflags argument
 *
 * Returns 1 when bit 0x1 is set, else 0. Soft bit test only; does not
 * call regexec(3). No parent wires.
 */
uint32_t
gj_regexec_has_notbol_u_7844(uint32_t u32Eflags)
{
	(void)NULL;
	return b7844_regexec_has_notbol(u32Eflags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_regexec_has_notbol_u_7844(uint32_t u32Eflags)
    __attribute__((alias("gj_regexec_has_notbol_u_7844")));
