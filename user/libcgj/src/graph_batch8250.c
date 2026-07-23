/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8250: byteswap / endian helpers batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8250(void);
 *     - Returns the compile-time graph batch number for this TU (8250).
 *   uint32_t __gj_batch_id_8250  (alias)
 *   __libcgj_batch8250_marker = "libcgj-batch8250"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers —
 * bswap16, bswap32, htons, ntohs, htonl, ntohl, le_id, be_id, swap_need,
 * batch_id). Unique gj_batch_id_8250 surface only; does NOT redefine
 * bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8250_marker[] = "libcgj-batch8250";

#define B8250_BATCH_ID  8250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8250_id(void)
{
	return B8250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8250 - report this TU's graph batch number.
 *
 * Always returns 8250. Soft continuum lamp for the 8241-8250
 * byteswap / endian helpers wave. No parent wires.
 */
uint32_t
gj_batch_id_8250(void)
{
	(void)NULL;
	return b8250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8250(void)
    __attribute__((alias("gj_batch_id_8250")));
