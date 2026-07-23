/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10158: seat soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat_ok_u_10158(void);
 *     - Returns 0 (seat soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live seat/device probe.
 *   uint32_t __gj_seat_ok_u_10158  (alias)
 *   __libcgj_batch10158_marker = "libcgj-batch10158"
 *
 * Exclusive CREATE-ONLY (10151-10160: polkit/seatd soft id stubs). Unique
 * gj_seat_ok_u_10158 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10158_marker[] = "libcgj-batch10158";

/* Soft lamp: seat ok stub (not asserted). */
#define B10158_SEAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10158_seat_ok(void)
{
	return B10158_SEAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat_ok_u_10158 - seat soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open seat devices,
 * claim VT seats, or probe seatd/logind seat objects. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_seat_ok_u_10158(void)
{
	(void)NULL;
	return b10158_seat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat_ok_u_10158(void)
    __attribute__((alias("gj_seat_ok_u_10158")));
