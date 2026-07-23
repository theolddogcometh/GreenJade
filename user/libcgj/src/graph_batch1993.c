/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1993: product-bar Deck Top 50 capability hint.
 *
 * Surface (unique symbols):
 *   int gj_product_bar_top50_hint(void);
 *     — Always returns 1. Placeholder capability bit for the product
 *       bar Deck Top 50 matrix row set (G-MAT-3 targeting). Soft
 *       compile-time presence probe only; does not claim title PASS.
 *   int __gj_product_bar_top50_hint  (alias)
 *   __libcgj_batch1993_marker = "libcgj-batch1993"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1993_marker[] = "libcgj-batch1993";

/* ---- freestanding helpers ---------------------------------------------- */

/* Placeholder capability bit: always set. */
static int
b1993_top50_cap(void)
{
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar_top50_hint — product-bar Deck Top 50 capability bit.
 *
 * Always returns 1 (placeholder). Distinct from steam/deck hints.
 */
int
gj_product_bar_top50_hint(void)
{
	(void)NULL;
	return b1993_top50_cap();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_product_bar_top50_hint(void)
    __attribute__((alias("gj_product_bar_top50_hint")));
