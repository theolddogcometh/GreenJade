/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5332: pure-value ACPI S3 sleep power state (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_state_s3_u(void);
 *     - Return the ACPI global power-state code for S3 (suspend-to-RAM).
 *       Always 3.
 *   uint32_t __gj_pwr_state_s3_u  (alias)
 *   __libcgj_batch5332_marker = "libcgj-batch5332"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_state_s3_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5332_marker[] = "libcgj-batch5332";

/* ACPI global system state S3 (suspend-to-RAM). */
#define B5332_S3  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5332_state_s3(void)
{
	return B5332_S3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_state_s3_u - ACPI S3 suspend-to-RAM power-state code.
 *
 * Always returns 3. Soft pure-data constant for power policy tables.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pwr_state_s3_u(void)
{
	(void)NULL;
	return b5332_state_s3();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_state_s3_u(void)
    __attribute__((alias("gj_pwr_state_s3_u")));
