/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7216: poll POLLERR presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_has_err_u_7216(uint32_t events);
 *     - Return 1 if events has POLLERR (0x008) set, else 0.
 *   uint32_t __gj_poll_has_err_u_7216  (alias)
 *   __libcgj_batch7216_marker = "libcgj-batch7216"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_has_err_u_7216 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7216_marker[] = "libcgj-batch7216";

/* POLLERR bit (0x008). */
#define B7216_POLLERR ((uint32_t)0x008u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7216_poll_has_err(uint32_t u32Events)
{
	return ((u32Events & B7216_POLLERR) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_has_err_u_7216 - 1 if events includes POLLERR.
 *
 * events: pollfd.events or pollfd.revents bitmask
 *
 * Returns 1 when bit 0x008 is set, else 0. Soft bit test only; does not
 * call poll. No parent wires.
 */
uint32_t
gj_poll_has_err_u_7216(uint32_t u32Events)
{
	(void)NULL;
	return b7216_poll_has_err(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_has_err_u_7216(uint32_t u32Events)
    __attribute__((alias("gj_poll_has_err_u_7216")));
