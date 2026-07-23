/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5789: seat0 active VT number.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat0_vt_nr_5789(void);
 *     - Returns stub active VT number for seat0 (1).
 *   uint32_t __gj_seat0_vt_nr_5789  (alias)
 *   __libcgj_batch5789_marker = "libcgj-batch5789"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_seat0_vt_nr_5789 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5789_marker[] = "libcgj-batch5789";

/* Active virtual terminal for seat0 in this stub wave. */
#define B5789_SEAT0_VT_NR  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5789_seat0_vt_nr(void)
{
	return B5789_SEAT0_VT_NR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat0_vt_nr_5789 - report stub active VT number for seat0.
 *
 * Always returns 1. Integer-only logind VT stub; does not open
 * /dev/tty0 or call VT_GETSTATE. Does not call libc. No parent wires.
 */
uint32_t
gj_seat0_vt_nr_5789(void)
{
	(void)NULL;
	return b5789_seat0_vt_nr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat0_vt_nr_5789(void)
    __attribute__((alias("gj_seat0_vt_nr_5789")));
