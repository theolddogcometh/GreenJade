/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2639: product path readiness mask constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_ready_mask(void);
 *     - Returns the product path readiness mask (always 0xff: low eight
 *       bits for rootfs marker, ld-gj name, steam bin, proton run,
 *       deck top50, game id, launch env, install media). Soft
 *       compile-time product tag for the 2631-2638 path wave.
 *   uint32_t __gj_product_ready_mask  (alias)
 *   __libcgj_batch2639_marker = "libcgj-batch2639"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_product_bar3_mask (batch2597 three-bit), gj_install_ready_bits
 * (batch2598 extract), and gj_product_bar3_bits (batch2398) — unique
 * gj_product_ready_mask surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2639_marker[] = "libcgj-batch2639";

/* Eight product path readiness bits (bits 0..7). */
#define B2639_READY_MASK  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2639_ready_mask(void)
{
	return B2639_READY_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_ready_mask - report the product path readiness mask.
 *
 * Always returns 0xff (low eight product-path slots). Link-time
 * presence of this symbol tags the path-wave readiness constant.
 * Pair with the 2631-2638 soft-ok helpers for slot semantics.
 * Does not call libc.
 */
uint32_t
gj_product_ready_mask(void)
{
	(void)NULL;
	return b2639_ready_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_ready_mask(void)
    __attribute__((alias("gj_product_ready_mask")));
