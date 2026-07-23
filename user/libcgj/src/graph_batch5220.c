/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5220: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5220(void);
 *     - Returns the compile-time graph batch number for this TU (5220).
 *   uint32_t __gj_batch_id_5220  (alias)
 *   __libcgj_batch5220_marker = "libcgj-batch5220"
 *
 * Exclusive continuum CREATE-ONLY (5211-5220: APIC unique —
 * apic_base_default_u, apic_id_reg_u, apic_ver_reg_u, apic_eoi_reg_u,
 * apic_svr_reg_u, apic_icr_lo_u, apic_icr_hi_u, apic_lvt_timer_u,
 * apic_is_x2_u, batch_id_5220). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5220_marker[] = "libcgj-batch5220";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5220_id(void)
{
	return 5220u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5220 - report this TU's graph batch number.
 *
 * Always returns 5220. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5220(void)
{
	(void)NULL;
	return b5220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5220(void)
    __attribute__((alias("gj_batch_id_5220")));
