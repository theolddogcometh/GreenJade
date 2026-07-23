/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5585: crash reporter enable stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crash_reporter_enabled_5585(void);
 *     - Returns 0 (crash reporter disabled). Integer-only stub; no
 *       minidump paths, no network upload.
 *   uint32_t __gj_crash_reporter_enabled_5585  (alias)
 *   __libcgj_batch5585_marker = "libcgj-batch5585"
 *
 * Exclusive continuum CREATE-ONLY (5581-5590: telemetry opt-out,
 * privacy gates, crash reporter stubs, product legal/ready flags).
 * Unique gj_crash_reporter_enabled_5585 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5585_marker[] = "libcgj-batch5585";

/* Crash reporter off by default (privacy-first stub). */
#define B5585_CRASH_REPORTER_ENABLED  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5585_enabled(void)
{
	return B5585_CRASH_REPORTER_ENABLED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crash_reporter_enabled_5585 - report crash-reporter enable lamp.
 *
 * Always returns 0 (disabled). Integer-only stub; does not install
 * signal handlers or write dumps. Does not call libc. No parent wires.
 */
uint32_t
gj_crash_reporter_enabled_5585(void)
{
	(void)NULL;
	return b5585_enabled();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crash_reporter_enabled_5585(void)
    __attribute__((alias("gj_crash_reporter_enabled_5585")));
