/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1992: product-bar Deck capability hint.
 *
 * Surface (unique symbols):
 *   int gj_product_bar_deck_hint(void);
 *     — Always returns 1. Placeholder capability bit for the product
 *       bar Steam Deck / handheld target row (Deck Top 50 path).
 *       Soft compile-time presence probe only; not a hardware Deck
 *       detection.
 *   int __gj_product_bar_deck_hint  (alias)
 *   __libcgj_batch1992_marker = "libcgj-batch1992"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1992_marker[] = "libcgj-batch1992";

/* ---- freestanding helpers ---------------------------------------------- */

/* Placeholder capability bit: always set. */
static int
b1992_deck_cap(void)
{
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar_deck_hint — product-bar Deck capability bit.
 *
 * Always returns 1 (placeholder). Distinct from
 * gj_product_bar_steam_hint so both bar slots can coexist.
 */
int
gj_product_bar_deck_hint(void)
{
	(void)NULL;
	return b1992_deck_cap();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_product_bar_deck_hint(void)
    __attribute__((alias("gj_product_bar_deck_hint")));
