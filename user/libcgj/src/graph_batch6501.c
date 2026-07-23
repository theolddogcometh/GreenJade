/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6501: bar3 install checklist deepen root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_checklist_deepen_root_6501(void);
 *     - Returns the soft bar3 install-checklist deepen root layout tag
 *       for this continuum (always 0x42494344, fourCC "BICD"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_bar3_install_checklist_deepen_root_6501  (alias)
 *   __libcgj_batch6501_marker = "libcgj-batch6501"
 *
 * Exclusive continuum CREATE-ONLY (6501-6510: post-6500 bar3 install
 * checklist deepen). Unique gj_bar3_install_checklist_deepen_root_6501
 * surface only; no multi-def. Distinct from
 * gj_product_install_deepen_root_6101,
 * gj_continuum_product_deepen_root_6301, and
 * gj_bar3_checklist_full_mask_5701. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6501_marker[] = "libcgj-batch6501";

/* Bar3 install checklist deepen root fourCC "BICD". */
#define B6501_CHECKLIST_DEEPEN_ROOT  0x42494344u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6501_root(void)
{
	return B6501_CHECKLIST_DEEPEN_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_checklist_deepen_root_6501 - BICD root layout tag.
 *
 * Always returns 0x42494344 ("BICD"). Soft pure-data layout tag for the
 * 6501-6510 continuum. Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_bar3_install_checklist_deepen_root_6501(void)
{
	(void)NULL;
	return b6501_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_checklist_deepen_root_6501(void)
    __attribute__((alias("gj_bar3_install_checklist_deepen_root_6501")));
