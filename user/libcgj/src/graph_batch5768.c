/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5768: interface full-duplex mode stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_duplex_full_5768(void);
 *     - Returns 1 (stub full-duplex negotiated). Soft product-connect
 *       duplex tag; not a live ethtool duplex query.
 *   uint32_t __gj_if_duplex_full_5768  (alias)
 *   __libcgj_batch5768_marker = "libcgj-batch5768"
 *
 * Exclusive continuum CREATE-ONLY (5761-5770: network interface
 * MTU/link/speed stubs for product connect). Unique
 * gj_if_duplex_full_5768 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5768_marker[] = "libcgj-batch5768";

/* Stub full-duplex lamp (1 = full duplex). */
#define B5768_IF_DUPLEX_FULL  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5768_duplex_full(void)
{
	return B5768_IF_DUPLEX_FULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_duplex_full_5768 - report stub interface full-duplex mode.
 *
 * Always returns 1 (full duplex). Integer-only product-connect duplex
 * stub; does not probe a real NIC. Does not call libc. No parent wires.
 */
uint32_t
gj_if_duplex_full_5768(void)
{
	(void)NULL;
	return b5768_duplex_full();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_duplex_full_5768(void)
    __attribute__((alias("gj_if_duplex_full_5768")));
