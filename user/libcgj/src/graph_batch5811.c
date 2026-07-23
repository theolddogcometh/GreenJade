/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5811: product bar3 steam bootstrap root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_bar3_boot_root_5811(void);
 *     - Returns the soft product bar3 / Steam bootstrap root layout tag
 *       for this continuum (always 0x50423331, fourCC "PB31"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_product_bar3_boot_root_5811  (alias)
 *   __libcgj_batch5811_marker = "libcgj-batch5811"
 *
 * Exclusive continuum CREATE-ONLY (5811-5820: product bar3 steam
 * bootstrap finalize). Unique gj_product_bar3_boot_root_5811 surface
 * only; no multi-def. Distinct from gj_steam_client_boot_root_5661 and
 * gj_product_bar3_mask (batch2597). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5811_marker[] = "libcgj-batch5811";

/* Product bar3 steam bootstrap root fourCC "PB31". */
#define B5811_BOOT_ROOT  0x50423331u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5811_boot_root(void)
{
	return B5811_BOOT_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar3_boot_root_5811 - product bar3 steam bootstrap root tag.
 *
 * Always returns 0x50423331 ("PB31"). Soft pure-data layout tag for the
 * 5811-5820 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_product_bar3_boot_root_5811(void)
{
	(void)NULL;
	return b5811_boot_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_bar3_boot_root_5811(void)
    __attribute__((alias("gj_product_bar3_boot_root_5811")));
