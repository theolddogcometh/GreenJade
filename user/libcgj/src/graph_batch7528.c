/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7528: waitid options validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_options_ok_u_7528(uint32_t options);
 *     - Return 1 if options uses only known waitid bits, else 0.
 *   uint32_t __gj_waitid_options_ok_u_7528  (alias)
 *   __libcgj_batch7528_marker = "libcgj-batch7528"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_options_ok_u_7528 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7528_marker[] = "libcgj-batch7528";

/*
 * Known waitid option bits (soft, no sys/wait.h):
 *   WNOHANG=1, WSTOPPED=2, WEXITED=4, WCONTINUED=8, WNOWAIT=0x01000000
 */
#define B7528_WAITID_KNOWN  (1u | 2u | 4u | 8u | 0x01000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7528_waitid_options_ok(uint32_t u32Options)
{
	return ((u32Options & ~B7528_WAITID_KNOWN) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_options_ok_u_7528 - 1 if options has no unknown bits.
 *
 * options: waitid(2) options argument (soft options-like u32)
 *
 * Returns 1 when only known waitid bits are set (including 0), else 0.
 * Soft catalog check; does not call waitid. No parent wires.
 */
uint32_t
gj_waitid_options_ok_u_7528(uint32_t u32Options)
{
	(void)NULL;
	return b7528_waitid_options_ok(u32Options);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_options_ok_u_7528(uint32_t u32Options)
    __attribute__((alias("gj_waitid_options_ok_u_7528")));
