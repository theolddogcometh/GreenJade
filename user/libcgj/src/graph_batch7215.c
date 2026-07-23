/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7215: poll POLLOUT presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_has_out_u_7215(uint32_t events);
 *     - Return 1 if events has POLLOUT (0x004) set, else 0.
 *   uint32_t __gj_poll_has_out_u_7215  (alias)
 *   __libcgj_batch7215_marker = "libcgj-batch7215"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_has_out_u_7215 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7215_marker[] = "libcgj-batch7215";

/* POLLOUT bit (0x004). */
#define B7215_POLLOUT ((uint32_t)0x004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7215_poll_has_out(uint32_t u32Events)
{
	return ((u32Events & B7215_POLLOUT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_has_out_u_7215 - 1 if events includes POLLOUT.
 *
 * events: pollfd.events or pollfd.revents bitmask
 *
 * Returns 1 when bit 0x004 is set, else 0. Soft bit test only; does not
 * call poll. No parent wires.
 */
uint32_t
gj_poll_has_out_u_7215(uint32_t u32Events)
{
	(void)NULL;
	return b7215_poll_has_out(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_has_out_u_7215(uint32_t u32Events)
    __attribute__((alias("gj_poll_has_out_u_7215")));
