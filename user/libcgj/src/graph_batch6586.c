/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6586: CPUID leaf needs ECX subleaf (soft).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_subleaf_need_6586(uint32_t leaf);
 *     - Returns 1 when leaf is in the soft "subleaf-required" catalog
 *       (4, 7, 0xB, 0xD, 0xF, 0x10, 0x12, 0x14, 0x17, 0x18, 0x1D, 0x1F),
 *       else 0. Pure-data hint; not a live enumeration of max subleaf.
 *   uint32_t __gj_cpuid_subleaf_need_6586  (alias)
 *   __libcgj_batch6586_marker = "libcgj-batch6586"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_subleaf_need_6586 surface only; no multi-def.
 * Distinct from leaf range ok (batch6584). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6586_marker[] = "libcgj-batch6586";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6586_subleaf_need(uint32_t uLeaf)
{
	switch (uLeaf) {
	case 0x04u: /* deterministic cache parameters */
	case 0x07u: /* structured extended feature flags */
	case 0x0Bu: /* extended topology */
	case 0x0Du: /* XSAVE features / size */
	case 0x0Fu: /* platform QoS monitoring */
	case 0x10u: /* platform QoS enforcement */
	case 0x12u: /* SGX capability */
	case 0x14u: /* Intel PT */
	case 0x17u: /* SoC vendor attributes */
	case 0x18u: /* deterministic address translation */
	case 0x1Du: /* AMX tile info */
	case 0x1Fu: /* V2 extended topology */
		return 1u;
	default:
		return 0u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_subleaf_need_6586 - soft ECX-subleaf-required leaf catalog.
 *
 * leaf: candidate basic CPUID leaf.
 * Returns 1 for known multi-subleaf leaves, else 0. Does not execute
 * CPUID. No parent wires.
 */
uint32_t
gj_cpuid_subleaf_need_6586(uint32_t uLeaf)
{
	(void)NULL;
	return b6586_subleaf_need(uLeaf);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_subleaf_need_6586(uint32_t uLeaf)
    __attribute__((alias("gj_cpuid_subleaf_need_6586")));
