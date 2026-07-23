/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5801: product bar3 install media root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_bar3_media_root_5801(void);
 *     - Returns the soft product bar3 / install-media root layout tag
 *       for this continuum (always 0x5042494D, fourCC "PBIM"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_product_bar3_media_root_5801  (alias)
 *   __libcgj_batch5801_marker = "libcgj-batch5801"
 *
 * Exclusive continuum CREATE-ONLY (5801-5810: product bar3 install
 * media finalize). Unique gj_product_bar3_media_root_5801 surface only;
 * no multi-def. Distinct from gj_product_bar3_boot_root_5811,
 * gj_install_media_ok (batch2638), and gj_product_bar3_mask
 * (batch2597). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5801_marker[] = "libcgj-batch5801";

/* Product bar3 install media root fourCC "PBIM". */
#define B5801_MEDIA_ROOT  0x5042494Du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5801_media_root(void)
{
	return B5801_MEDIA_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar3_media_root_5801 - product bar3 install media root tag.
 *
 * Always returns 0x5042494D ("PBIM"). Soft pure-data layout tag for the
 * 5801-5810 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_product_bar3_media_root_5801(void)
{
	(void)NULL;
	return b5801_media_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_bar3_media_root_5801(void)
    __attribute__((alias("gj_product_bar3_media_root_5801")));
