/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7219: poll events errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_events_errorish_u_7219(uint32_t events);
 *     - Return 1 if events has any unknown bits outside known POLL*
 *       mask (0x00d), else 0.
 *   uint32_t __gj_poll_events_errorish_u_7219  (alias)
 *   __libcgj_batch7219_marker = "libcgj-batch7219"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_events_errorish_u_7219 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7219_marker[] = "libcgj-batch7219";

/*
 * Known poll event bits for this continuum:
 *   POLLIN | POLLOUT | POLLERR = 0x00d
 */
#define B7219_POLL_KNOWN_MASK ((uint32_t)0x00du)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7219_poll_events_errorish(uint32_t u32Events)
{
	return ((u32Events & ~B7219_POLL_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_events_errorish_u_7219 - 1 if events has unknown bits.
 *
 * events: pollfd.events or pollfd.revents bitmask
 *
 * Returns 1 when any bit outside {0x001, 0x004, 0x008} is set, else 0.
 * Soft inverse of events_ok; does not call poll. No parent wires.
 */
uint32_t
gj_poll_events_errorish_u_7219(uint32_t u32Events)
{
	(void)NULL;
	return b7219_poll_events_errorish(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_events_errorish_u_7219(uint32_t u32Events)
    __attribute__((alias("gj_poll_events_errorish_u_7219")));
