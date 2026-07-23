/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5785: seat0 active lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat0_active_5785(void);
 *     - Returns 1 (seat0 is the active seat).
 *   uint32_t __gj_seat0_active_5785  (alias)
 *   __libcgj_batch5785_marker = "libcgj-batch5785"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_seat0_active_5785 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5785_marker[] = "libcgj-batch5785";

/* seat0 is the active local seat in this stub wave. */
#define B5785_SEAT0_ACTIVE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5785_seat0_active(void)
{
	return B5785_SEAT0_ACTIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat0_active_5785 - report seat0 active lamp.
 *
 * Always returns 1 (active). Integer-only logind seat stub; does not
 * call sd_seat_get_active or switch VTs. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_seat0_active_5785(void)
{
	(void)NULL;
	return b5785_seat0_active();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat0_active_5785(void)
    __attribute__((alias("gj_seat0_active_5785")));
