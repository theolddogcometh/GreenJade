/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5217: pure-value local APIC ICR high offset (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_apic_icr_hi_u(void);
 *     - Return the xAPIC Interrupt Command Register high dword MMIO
 *       offset 0x310.
 *   uint32_t __gj_apic_icr_hi_u  (alias)
 *   __libcgj_batch5217_marker = "libcgj-batch5217"
 *
 * Exclusive continuum CREATE-ONLY (5211-5220: APIC unique —
 * apic_base_default_u, apic_id_reg_u, apic_ver_reg_u, apic_eoi_reg_u,
 * apic_svr_reg_u, apic_icr_lo_u, apic_icr_hi_u, apic_lvt_timer_u,
 * apic_is_x2_u, batch_id_5220). Unique gj_apic_icr_hi_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5217_marker[] = "libcgj-batch5217";

/* Local APIC ICR high dword offset within the xAPIC MMIO window. */
#define B5217_APIC_ICR_HI  0x310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5217_icr_hi(void)
{
	return B5217_APIC_ICR_HI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_apic_icr_hi_u - report Local APIC ICR high dword MMIO offset.
 *
 * Always returns 0x310. Soft compile-time register offset constant; does
 * not touch MMIO. Self-contained; no parent wires.
 */
uint32_t
gj_apic_icr_hi_u(void)
{
	(void)NULL;
	return b5217_icr_hi();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_apic_icr_hi_u(void)
    __attribute__((alias("gj_apic_icr_hi_u")));
