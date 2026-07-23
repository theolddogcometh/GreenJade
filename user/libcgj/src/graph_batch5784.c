/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5784: seat0 present lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat0_present_5784(void);
 *     - Returns 1 (seat0 is present in the stub seat list).
 *   uint32_t __gj_seat0_present_5784  (alias)
 *   __libcgj_batch5784_marker = "libcgj-batch5784"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_seat0_present_5784 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5784_marker[] = "libcgj-batch5784";

/* seat0 exists in the freestanding seat topology. */
#define B5784_SEAT0_PRESENT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5784_seat0_present(void)
{
	return B5784_SEAT0_PRESENT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat0_present_5784 - report seat0 present lamp.
 *
 * Always returns 1 (present). Soft pure-data product tag; does not
 * scan /sys/devices or open a logind seat object. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_seat0_present_5784(void)
{
	(void)NULL;
	return b5784_seat0_present();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat0_present_5784(void)
    __attribute__((alias("gj_seat0_present_5784")));
