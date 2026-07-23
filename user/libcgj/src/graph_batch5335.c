/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5335: power-state sleep predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_is_sleep_u(uint32_t s);
 *     - Return 1 if s is a sleep global state (S3 or S4), else 0.
 *       S3 = 3 (suspend-to-RAM), S4 = 4 (hibernate).
 *   uint32_t __gj_pwr_is_sleep_u  (alias)
 *   __libcgj_batch5335_marker = "libcgj-batch5335"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_is_sleep_u surface
 * only; no multi-def. Complements pwr_state_s3_u / pwr_state_s4_u
 * without parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5335_marker[] = "libcgj-batch5335";

/* ACPI S3 suspend-to-RAM. */
#define B5335_S3  3u
/* ACPI S4 hibernate. */
#define B5335_S4  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5335_is_sleep(uint32_t u32S)
{
	if (u32S == B5335_S3 || u32S == B5335_S4) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_is_sleep_u - 1 if power state s is S3 or S4 (sleep family).
 *
 * s: ACPI-style global system state code
 *
 * Returns 1 when s == 3 or s == 4, else 0. Self-contained; does not
 * call gj_pwr_state_s3_u / gj_pwr_state_s4_u. No parent wires.
 */
uint32_t
gj_pwr_is_sleep_u(uint32_t u32S)
{
	(void)NULL;
	return b5335_is_sleep(u32S);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_is_sleep_u(uint32_t u32S)
    __attribute__((alias("gj_pwr_is_sleep_u")));
