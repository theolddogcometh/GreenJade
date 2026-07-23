/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5584: privacy data-retention days.
 *
 * Surface (unique symbols):
 *   uint32_t gj_privacy_retention_days_5584(void);
 *     - Returns 0 (no local retention of privacy-sensitive telemetry
 *       or crash payloads). Soft pure-data retention cap.
 *   uint32_t __gj_privacy_retention_days_5584  (alias)
 *   __libcgj_batch5584_marker = "libcgj-batch5584"
 *
 * Exclusive continuum CREATE-ONLY (5581-5590: telemetry opt-out,
 * privacy gates, crash reporter stubs, product legal/ready flags).
 * Unique gj_privacy_retention_days_5584 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5584_marker[] = "libcgj-batch5584";

/* Retention days: 0 = do not retain privacy-sensitive payloads. */
#define B5584_RETENTION_DAYS  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5584_retention_days(void)
{
	return B5584_RETENTION_DAYS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_privacy_retention_days_5584 - report privacy retention cap (days).
 *
 * Always returns 0 (no retention). Soft pure-data only; does not
 * walk disk. Does not call libc. No parent wires.
 */
uint32_t
gj_privacy_retention_days_5584(void)
{
	(void)NULL;
	return b5584_retention_days();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_privacy_retention_days_5584(void)
    __attribute__((alias("gj_privacy_retention_days_5584")));
