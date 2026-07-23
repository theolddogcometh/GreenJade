/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5701: bar3 install checklist full mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_checklist_full_mask_5701(void);
 *     - Returns 0xF (all four product bar3 install-checklist bits:
 *       steam/deck/top50/install). Soft compile-time checklist mask.
 *   uint32_t __gj_bar3_checklist_full_mask_5701  (alias)
 *   __libcgj_batch5701_marker = "libcgj-batch5701"
 *
 * Exclusive continuum CREATE-ONLY (5701-5710: bar3 install checklist
 * bits). Unique gj_bar3_checklist_full_mask_5701 surface only; no
 * multi-def. Distinct from gj_product_bar3_bits (batch2398) and
 * gj_install_ready_bits (batch2598). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5701_marker[] = "libcgj-batch5701";

/* Full bar3 install checklist: steam|deck|top50|install (bits 0..3). */
#define B5701_CHECKLIST_FULL  0xFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5701_full_mask(void)
{
	return B5701_CHECKLIST_FULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_checklist_full_mask_5701 - full product bar3 checklist mask.
 *
 * Always returns 0xF (four install-checklist bits). Soft pure-data
 * constant for pairing with extract/set/popcount helpers in this
 * wave. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_checklist_full_mask_5701(void)
{
	(void)NULL;
	return b5701_full_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_checklist_full_mask_5701(void)
    __attribute__((alias("gj_bar3_checklist_full_mask_5701")));
