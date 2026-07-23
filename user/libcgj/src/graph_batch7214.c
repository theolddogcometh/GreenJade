/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7214: poll POLLIN presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_has_in_u_7214(uint32_t events);
 *     - Return 1 if events has POLLIN (0x001) set, else 0.
 *   uint32_t __gj_poll_has_in_u_7214  (alias)
 *   __libcgj_batch7214_marker = "libcgj-batch7214"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_has_in_u_7214 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7214_marker[] = "libcgj-batch7214";

/* POLLIN bit (0x001). */
#define B7214_POLLIN ((uint32_t)0x001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7214_poll_has_in(uint32_t u32Events)
{
	return ((u32Events & B7214_POLLIN) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_has_in_u_7214 - 1 if events includes POLLIN.
 *
 * events: pollfd.events or pollfd.revents bitmask
 *
 * Returns 1 when bit 0x001 is set, else 0. Soft bit test only; does not
 * call poll. No parent wires.
 */
uint32_t
gj_poll_has_in_u_7214(uint32_t u32Events)
{
	(void)NULL;
	return b7214_poll_has_in(u32Events);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_has_in_u_7214(uint32_t u32Events)
    __attribute__((alias("gj_poll_has_in_u_7214")));
