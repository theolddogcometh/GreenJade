/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1991: product-bar Steam capability hint.
 *
 * Surface (unique symbols):
 *   int gj_product_bar_steam_hint(void);
 *     — Always returns 1. Placeholder capability bit for the product
 *       bar Steam host-side install path (matrix adoption plan).
 *       Soft compile-time presence probe only; not a runtime Steam
 *       install check.
 *   int __gj_product_bar_steam_hint  (alias)
 *   __libcgj_batch1991_marker = "libcgj-batch1991"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1991_marker[] = "libcgj-batch1991";

/* ---- freestanding helpers ---------------------------------------------- */

/* Placeholder capability bit: always set. */
static int
b1991_steam_cap(void)
{
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar_steam_hint — product-bar Steam capability bit.
 *
 * Always returns 1 (placeholder). Link-time presence of this symbol
 * means the Steam product-bar slot is wired into the continuum.
 */
int
gj_product_bar_steam_hint(void)
{
	(void)NULL;
	return b1991_steam_cap();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_product_bar_steam_hint(void)
    __attribute__((alias("gj_product_bar_steam_hint")));
