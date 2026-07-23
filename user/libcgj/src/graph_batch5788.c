/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5788: seat0 can_graphical lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat_can_graphical_5788(void);
 *     - Returns 1 (seat0 can host a graphical session).
 *   uint32_t __gj_seat_can_graphical_5788  (alias)
 *   __libcgj_batch5788_marker = "libcgj-batch5788"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_seat_can_graphical_5788
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5788_marker[] = "libcgj-batch5788";

/* seat0 has graphical capability in the freestanding stub. */
#define B5788_SEAT_CAN_GRAPHICAL  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5788_seat_can_graphical(void)
{
	return B5788_SEAT_CAN_GRAPHICAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat_can_graphical_5788 - report seat0 CanGraphical lamp.
 *
 * Always returns 1 (can graphical). Soft pure-data product tag; does
 * not probe DRM devices or query logind Seat.CanGraphical. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_seat_can_graphical_5788(void)
{
	(void)NULL;
	return b5788_seat_can_graphical();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat_can_graphical_5788(void)
    __attribute__((alias("gj_seat_can_graphical_5788")));
