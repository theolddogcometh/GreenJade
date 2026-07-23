/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7527: waitid WNOHANG presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_waitid_has_wnohang_u_7527(uint32_t options);
 *     - Return 1 if options has WNOHANG (1), else 0.
 *   uint32_t __gj_waitid_has_wnohang_u_7527  (alias)
 *   __libcgj_batch7527_marker = "libcgj-batch7527"
 *
 * Exclusive continuum CREATE-ONLY (7521-7530: waitid options stubs —
 * wnohang_id, wexited_id, wstopped_id, wcontinued_id, wnowait_id,
 * has_wexited, has_wnohang, options_ok, options_errorish,
 * batch_id_7530).
 * Unique gj_waitid_has_wnohang_u_7527 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7527_marker[] = "libcgj-batch7527";

/* Soft waitid WNOHANG bit. */
#define B7527_WAITID_WNOHANG  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7527_waitid_has_wnohang(uint32_t u32Options)
{
	return ((u32Options & B7527_WAITID_WNOHANG) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_waitid_has_wnohang_u_7527 - 1 if options includes WNOHANG.
 *
 * options: waitid(2) options argument (soft options-like u32)
 *
 * Returns 1 when bit 1 is set, else 0. Soft bit test only;
 * does not call waitid. No parent wires.
 */
uint32_t
gj_waitid_has_wnohang_u_7527(uint32_t u32Options)
{
	(void)NULL;
	return b7527_waitid_has_wnohang(u32Options);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_waitid_has_wnohang_u_7527(uint32_t u32Options)
    __attribute__((alias("gj_waitid_has_wnohang_u_7527")));
