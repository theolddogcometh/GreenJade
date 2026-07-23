/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6901: bar3 product deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_root_6901(void);
 *     - Returns the soft bar3 product-deepen root layout tag for this
 *       continuum (always 0x4250444E, fourCC "BPDN"). Not a filesystem
 *       path probe.
 *   uint32_t __gj_bar3_product_deepen_root_6901  (alias)
 *   __libcgj_batch6901_marker = "libcgj-batch6901"
 *
 * Exclusive continuum CREATE-ONLY (6901-6910: post-6900 bar3 product
 * deepen). Unique gj_bar3_product_deepen_root_6901 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_root_6701,
 * gj_bar3_install_checklist_deepen_root_6501, and
 * gj_continuum_product_deepen_root_6601. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6901_marker[] = "libcgj-batch6901";

/* Bar3 product deepen root fourCC "BPDN". */
#define B6901_BPD_ROOT  0x4250444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6901_root(void)
{
	return B6901_BPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_root_6901 - bar3 product deepen root.
 *
 * Always returns 0x4250444E ("BPDN"). Soft pure-data layout tag for the
 * 6901-6910 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_bar3_product_deepen_root_6901(void)
{
	(void)NULL;
	return b6901_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_root_6901(void)
    __attribute__((alias("gj_bar3_product_deepen_root_6901")));
