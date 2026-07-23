/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5782: seat count (seat0-only stub).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat_count_5782(void);
 *     - Returns stub seat count (1 = seat0 only).
 *   uint32_t __gj_seat_count_5782  (alias)
 *   __libcgj_batch5782_marker = "libcgj-batch5782"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_seat_count_5782 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5782_marker[] = "libcgj-batch5782";

/* Single local seat (seat0) in the freestanding stub topology. */
#define B5782_SEAT_COUNT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5782_seat_count(void)
{
	return B5782_SEAT_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat_count_5782 - report stub logind seat list length.
 *
 * Always returns 1 (seat0 only). Integer-only stub; does not enumerate
 * /sys/fs/cgroup or talk to logind. Does not call libc. No parent wires.
 */
uint32_t
gj_seat_count_5782(void)
{
	(void)NULL;
	return b5782_seat_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat_count_5782(void)
    __attribute__((alias("gj_seat_count_5782")));
