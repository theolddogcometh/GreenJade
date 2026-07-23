/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7217: poll events validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_events_ok_u_7217(uint32_t events);
 *     - Return 1 if events uses only known POLL* bits
 *       (0x001|0x004|0x008), else 0.
 *   uint32_t __gj_poll_events_ok_u_7217  (alias)
 *   __libcgj_batch7217_marker = "libcgj-batch7217"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_events_ok_u_7217 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7217_marker[] = "libcgj-batch7217";

/*
 * Known poll event bits for this continuum:
 *   POLLIN (0x001) | POLLOUT (0x004) | POLLERR (0x008) = 0x00d
 */
#define B7217_POLL_KNOWN_MASK ((uint32_t)0x00du)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7217_poll_events_ok(uint32_t u32Events)
{
	return ((u32Events & ~B7217_POLL_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_events_ok_u_7217 - 1 if events has no unknown bits.
 *
 * events: pollfd.events or pollfd.revents bitmask
 *
 * Returns 1 when only bits in {0x001, 0x004, 0x008} are set (including
 * 0), else 0. Soft catalog check; does not call poll. No parent wires.
 */
uint32_t
gj_poll_events_ok_u_7217(uint32_t u32Events)
{
	(void)NULL;
	return b7217_poll_events_ok(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_events_ok_u_7217(uint32_t u32Events)
    __attribute__((alias("gj_poll_events_ok_u_7217")));
