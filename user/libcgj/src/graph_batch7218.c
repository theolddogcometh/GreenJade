/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7218: poll events pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_events_pack_u_7218(uint32_t want_in,
 *                                       uint32_t want_out,
 *                                       uint32_t want_err);
 *     - Pack soft POLL* wants into an events mask (0x001|0x004|0x008).
 *   uint32_t __gj_poll_events_pack_u_7218  (alias)
 *   __libcgj_batch7218_marker = "libcgj-batch7218"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_events_pack_u_7218 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7218_marker[] = "libcgj-batch7218";

/* Soft pack bits matching POSIX/glibc POLL* layout. */
#define B7218_POLLIN  ((uint32_t)0x001u)
#define B7218_POLLOUT ((uint32_t)0x004u)
#define B7218_POLLERR ((uint32_t)0x008u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7218_poll_events_pack(uint32_t u32WantIn,
                       uint32_t u32WantOut,
                       uint32_t u32WantErr)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantIn != 0u)
		u32Pack |= B7218_POLLIN;
	if (u32WantOut != 0u)
		u32Pack |= B7218_POLLOUT;
	if (u32WantErr != 0u)
		u32Pack |= B7218_POLLERR;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_events_pack_u_7218 - soft-pack poll event wants.
 *
 * want_in:  non-zero to set POLLIN  (0x001)
 * want_out: non-zero to set POLLOUT (0x004)
 * want_err: non-zero to set POLLERR (0x008)
 *
 * Returns bitmask of requested POLL* bits. Pure integer pack; does not
 * call poll. No parent wires.
 */
uint32_t
gj_poll_events_pack_u_7218(uint32_t u32WantIn,
                           uint32_t u32WantOut,
                           uint32_t u32WantErr)
{
	(void)NULL;
	return b7218_poll_events_pack(u32WantIn, u32WantOut, u32WantErr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_events_pack_u_7218(uint32_t u32WantIn,
                                      uint32_t u32WantOut,
                                      uint32_t u32WantErr)
    __attribute__((alias("gj_poll_events_pack_u_7218")));
