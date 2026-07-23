/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5211: pure-value local APIC default MMIO base (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_apic_base_default_u(void);
 *     - Return the classic xAPIC local APIC default physical base
 *       0xFEE00000 (Intel SDM default when IA32_APIC_BASE is reset).
 *   uint32_t __gj_apic_base_default_u  (alias)
 *   __libcgj_batch5211_marker = "libcgj-batch5211"
 *
 * Exclusive continuum CREATE-ONLY (5211-5220: APIC unique —
 * apic_base_default_u, apic_id_reg_u, apic_ver_reg_u, apic_eoi_reg_u,
 * apic_svr_reg_u, apic_icr_lo_u, apic_icr_hi_u, apic_lvt_timer_u,
 * apic_is_x2_u, batch_id_5220). Unique gj_apic_base_default_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5211_marker[] = "libcgj-batch5211";

/* Classic local APIC default physical base (xAPIC MMIO window). */
#define B5211_APIC_BASE_DEFAULT  0xFEE00000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5211_base_default(void)
{
	return B5211_APIC_BASE_DEFAULT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_apic_base_default_u - report default local APIC physical base.
 *
 * Always returns 0xFEE00000. Soft compile-time MMIO base constant; does
 * not read IA32_APIC_BASE MSR. Self-contained; no parent wires.
 */
uint32_t
gj_apic_base_default_u(void)
{
	(void)NULL;
	return b5211_base_default();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_apic_base_default_u(void)
    __attribute__((alias("gj_apic_base_default_u")));
