/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5331: pure-value ACPI S0 working power state (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_state_s0_u(void);
 *     - Return the ACPI global power-state code for S0 (working / full-on).
 *       Always 0.
 *   uint32_t __gj_pwr_state_s0_u  (alias)
 *   __libcgj_batch5331_marker = "libcgj-batch5331"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_state_s0_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5331_marker[] = "libcgj-batch5331";

/* ACPI global system state S0 (working). */
#define B5331_S0  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5331_state_s0(void)
{
	return B5331_S0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_state_s0_u - ACPI S0 working power-state code.
 *
 * Always returns 0. Soft pure-data constant for power policy tables.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pwr_state_s0_u(void)
{
	(void)NULL;
	return b5331_state_s0();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_state_s0_u(void)
    __attribute__((alias("gj_pwr_state_s0_u")));
