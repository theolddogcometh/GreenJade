/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5218: pure-value local APIC LVT Timer offset (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_apic_lvt_timer_u(void);
 *     - Return the xAPIC Local Vector Table Timer entry MMIO offset 0x320.
 *   uint32_t __gj_apic_lvt_timer_u  (alias)
 *   __libcgj_batch5218_marker = "libcgj-batch5218"
 *
 * Exclusive continuum CREATE-ONLY (5211-5220: APIC unique —
 * apic_base_default_u, apic_id_reg_u, apic_ver_reg_u, apic_eoi_reg_u,
 * apic_svr_reg_u, apic_icr_lo_u, apic_icr_hi_u, apic_lvt_timer_u,
 * apic_is_x2_u, batch_id_5220). Unique gj_apic_lvt_timer_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5218_marker[] = "libcgj-batch5218";

/* Local APIC LVT Timer register offset within the xAPIC MMIO window. */
#define B5218_APIC_LVT_TIMER  0x320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5218_lvt_timer(void)
{
	return B5218_APIC_LVT_TIMER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_apic_lvt_timer_u - report Local APIC LVT Timer MMIO offset.
 *
 * Always returns 0x320. Soft compile-time register offset constant; does
 * not touch MMIO. Self-contained; no parent wires.
 */
uint32_t
gj_apic_lvt_timer_u(void)
{
	(void)NULL;
	return b5218_lvt_timer();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_apic_lvt_timer_u(void)
    __attribute__((alias("gj_apic_lvt_timer_u")));
