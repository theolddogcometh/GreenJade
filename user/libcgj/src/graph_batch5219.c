/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5219: pure-value x2APIC feature soft bit check (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_apic_is_x2_u(uint32_t ver);
 *     - Soft bit check: return 1 if ver bit 21 is set (CPUID.01H:ECX[21]
 *       x2APIC feature flag layout), else 0. Does not execute CPUID.
 *   uint32_t __gj_apic_is_x2_u  (alias)
 *   __libcgj_batch5219_marker = "libcgj-batch5219"
 *
 * Exclusive continuum CREATE-ONLY (5211-5220: APIC unique —
 * apic_base_default_u, apic_id_reg_u, apic_ver_reg_u, apic_eoi_reg_u,
 * apic_svr_reg_u, apic_icr_lo_u, apic_icr_hi_u, apic_lvt_timer_u,
 * apic_is_x2_u, batch_id_5220). Unique gj_apic_is_x2_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5219_marker[] = "libcgj-batch5219";

/* CPUID.01H:ECX[21] x2APIC feature bit (soft layout probe). */
#define B5219_X2APIC_BIT  (1u << 21)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5219_is_x2(uint32_t u32Ver)
{
	if ((u32Ver & B5219_X2APIC_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_apic_is_x2_u - soft 1/0 check for x2APIC feature bit 21 in ver.
 *
 * ver: feature-word style value (e.g. CPUID leaf 1 ECX layout). Bit 21
 *      is the classic x2APIC present flag. Soft only — no CPUID, no MSR.
 *
 * Returns 1 when bit 21 is set, else 0. Self-contained; no parent wires.
 */
uint32_t
gj_apic_is_x2_u(uint32_t u32Ver)
{
	(void)NULL;
	return b5219_is_x2(u32Ver);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_apic_is_x2_u(uint32_t u32Ver)
    __attribute__((alias("gj_apic_is_x2_u")));
