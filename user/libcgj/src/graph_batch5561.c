/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5561: NVMe SMART composite temperature stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_smart_temp_u(uint32_t kelvin);
 *     - Convert SMART composite temperature from Kelvin to Celsius-style
 *       units: kelvin >= 273 → kelvin - 273, else 0. Stub only; no I/O.
 *   uint32_t __gj_nvme_smart_temp_u  (alias)
 *   __libcgj_batch5561_marker = "libcgj-batch5561"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_nvme_smart_temp_u surface only; no
 * multi-def. Distinct from prior nvme opcode/size waves (5251-5260).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5561_marker[] = "libcgj-batch5561";

/* Absolute zero offset for Celsius-style conversion (Kelvin). */
#define B5561_KELVIN_ZERO  273u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5561_temp_c(uint32_t u32Kelvin)
{
	if (u32Kelvin < B5561_KELVIN_ZERO) {
		return 0u;
	}
	return u32Kelvin - B5561_KELVIN_ZERO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_smart_temp_u - NVMe SMART composite temp Kelvin→°C stub.
 *
 * kelvin: SMART log composite temperature in Kelvin
 *
 * Returns Celsius-style units (kelvin - 273) when kelvin >= 273,
 * else 0. Integer-only stub; no device access. No parent wires.
 */
uint32_t
gj_nvme_smart_temp_u(uint32_t u32Kelvin)
{
	(void)NULL;
	return b5561_temp_c(u32Kelvin);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_smart_temp_u(uint32_t u32Kelvin)
    __attribute__((alias("gj_nvme_smart_temp_u")));
