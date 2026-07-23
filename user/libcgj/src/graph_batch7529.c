/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7529: waitid options errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_options_errorish_u_7529(uint32_t options);
 *     - Return 1 if options has any bits outside known waitid mask.
 *   uint32_t __gj_waitid_options_errorish_u_7529  (alias)
 *   __libcgj_batch7529_marker = "libcgj-batch7529"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_options_errorish_u_7529 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7529_marker[] = "libcgj-batch7529";

/*
 * Soft known waitid options mask. Bits outside this are errorish.
 *   WNOHANG|WSTOPPED|WEXITED|WCONTINUED|WNOWAIT
 */
#define B7529_WAITID_KNOWN  (1u | 2u | 4u | 8u | 0x01000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7529_waitid_options_errorish(uint32_t u32Options)
{
	return ((u32Options & ~B7529_WAITID_KNOWN) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_options_errorish_u_7529 - 1 if options has unknown bits.
 *
 * options: waitid(2) options argument (soft options-like u32)
 *
 * Returns 1 when any bit outside the known waitid mask is set, else 0.
 * Soft inverse of options_ok; does not call waitid. No parent wires.
 */
uint32_t
gj_waitid_options_errorish_u_7529(uint32_t u32Options)
{
	(void)NULL;
	return b7529_waitid_options_errorish(u32Options);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_options_errorish_u_7529(uint32_t u32Options)
    __attribute__((alias("gj_waitid_options_errorish_u_7529")));
