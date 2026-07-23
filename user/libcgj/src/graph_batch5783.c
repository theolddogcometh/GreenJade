/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5783: seat0 ordinal id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat0_id_5783(void);
 *     - Returns stub seat0 ordinal id (0).
 *   uint32_t __gj_seat0_id_5783  (alias)
 *   __libcgj_batch5783_marker = "libcgj-batch5783"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_seat0_id_5783 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5783_marker[] = "libcgj-batch5783";

/* seat0 is the zero-ordinal local seat. */
#define B5783_SEAT0_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5783_seat0_id(void)
{
	return B5783_SEAT0_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat0_id_5783 - report stub seat0 ordinal identifier.
 *
 * Always returns 0. Integer-only seat tag; does not parse the string
 * "seat0" or query logind. Does not call libc. No parent wires.
 */
uint32_t
gj_seat0_id_5783(void)
{
	(void)NULL;
	return b5783_seat0_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat0_id_5783(void)
    __attribute__((alias("gj_seat0_id_5783")));
