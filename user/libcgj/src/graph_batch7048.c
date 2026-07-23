/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7048: io_pgetevents soft arg request pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_pack_u_7048(uint32_t want_timeout,
 *                                         uint32_t want_sigmask);
 *     - Pack soft timeout/sigmask request flags into a 2-bit mask.
 *   uint32_t __gj_io_pgetevents_pack_u_7048  (alias)
 *   __libcgj_batch7048_marker = "libcgj-batch7048"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique gj_io_pgetevents_pack_u_7048
 * surface only; no multi-def. Does NOT redefine io_pgetevents.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7048_marker[] = "libcgj-batch7048";

/* Soft request bits: bit0 = timeout, bit1 = sigmask. */
#define B7048_WANT_TIMEOUT ((uint32_t)0x1u)
#define B7048_WANT_SIGMASK ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7048_io_pgetevents_pack(uint32_t u32WantTimeout, uint32_t u32WantSigmask)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantTimeout != 0u)
		u32Pack |= B7048_WANT_TIMEOUT;
	if (u32WantSigmask != 0u)
		u32Pack |= B7048_WANT_SIGMASK;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_pack_u_7048 - soft-pack timeout/sigmask wants.
 *
 * want_timeout: non-zero to request a finite timeout
 * want_sigmask: non-zero to request a temporary signal mask
 *
 * Returns bitmask: bit0=timeout, bit1=sigmask. Pure integer pack;
 * does not call io_pgetevents. No parent wires.
 */
uint32_t
gj_io_pgetevents_pack_u_7048(uint32_t u32WantTimeout, uint32_t u32WantSigmask)
{
	(void)NULL;
	return b7048_io_pgetevents_pack(u32WantTimeout, u32WantSigmask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_pack_u_7048(uint32_t u32WantTimeout,
                                        uint32_t u32WantSigmask)
    __attribute__((alias("gj_io_pgetevents_pack_u_7048")));
