/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5334: pure-value ACPI S5 soft-off power state (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_state_s5_u(void);
 *     - Return the ACPI global power-state code for S5 (soft off).
 *       Always 5.
 *   uint32_t __gj_pwr_state_s5_u  (alias)
 *   __libcgj_batch5334_marker = "libcgj-batch5334"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_state_s5_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5334_marker[] = "libcgj-batch5334";

/* ACPI global system state S5 (soft off). */
#define B5334_S5  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5334_state_s5(void)
{
	return B5334_S5;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_state_s5_u - ACPI S5 soft-off power-state code.
 *
 * Always returns 5. Soft pure-data constant for power policy tables.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pwr_state_s5_u(void)
{
	(void)NULL;
	return b5334_state_s5();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_state_s5_u(void)
    __attribute__((alias("gj_pwr_state_s5_u")));
