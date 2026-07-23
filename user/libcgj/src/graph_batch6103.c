/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6103: product install deepen ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_deepen_ready_6103(void);
 *     - Returns 1 (install-deepen surface ready for the post-6100
 *       product deepen install continuum). Soft compile-time product
 *       tag; not a live block-device or ISO path probe.
 *   uint32_t __gj_product_install_deepen_ready_6103  (alias)
 *   __libcgj_batch6103_marker = "libcgj-batch6103"
 *
 * Exclusive continuum CREATE-ONLY (6101-6110: post-6100 product
 * deepen install). Unique gj_product_install_deepen_ready_6103 surface
 * only; no multi-def. Distinct from gj_install_media_ready_5803 and
 * gj_install_media_ready_5404. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6103_marker[] = "libcgj-batch6103";

/* Install deepen surface ready lamp. */
#define B6103_DEEPEN_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6103_ready(void)
{
	return B6103_DEEPEN_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_deepen_ready_6103 - install-deepen ready lamp.
 *
 * Always returns 1 (surface ready). Soft pure-data product tag.
 * Does not probe devices or call libc. No parent wires.
 */
uint32_t
gj_product_install_deepen_ready_6103(void)
{
	(void)NULL;
	return b6103_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_install_deepen_ready_6103(void)
    __attribute__((alias("gj_product_install_deepen_ready_6103")));
