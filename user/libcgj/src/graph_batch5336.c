/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5336: power-state soft-off predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_is_off_u(uint32_t s);
 *     - Return 1 if s is the soft-off global state (S5 == 5), else 0.
 *   uint32_t __gj_pwr_is_off_u  (alias)
 *   __libcgj_batch5336_marker = "libcgj-batch5336"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_is_off_u surface
 * only; no multi-def. Complements pwr_state_s5_u without parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5336_marker[] = "libcgj-batch5336";

/* ACPI S5 soft off. */
#define B5336_S5  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5336_is_off(uint32_t u32S)
{
	if (u32S == B5336_S5) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_is_off_u - 1 if power state s is S5 (soft off).
 *
 * s: ACPI-style global system state code
 *
 * Returns 1 when s == 5, else 0. Self-contained; does not call
 * gj_pwr_state_s5_u. No parent wires.
 */
uint32_t
gj_pwr_is_off_u(uint32_t u32S)
{
	(void)NULL;
	return b5336_is_off(u32S);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_is_off_u(uint32_t u32S)
    __attribute__((alias("gj_pwr_is_off_u")));
