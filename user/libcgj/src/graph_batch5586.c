/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5586: crash reporter severity floor stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crash_reporter_severity_5586(void);
 *     - Returns 0xFFFFFFFFu (maximum severity floor — effectively
 *       admits no reports when paired with a disabled reporter).
 *       Integer-only stub.
 *   uint32_t __gj_crash_reporter_severity_5586  (alias)
 *   __libcgj_batch5586_marker = "libcgj-batch5586"
 *
 * Exclusive continuum CREATE-ONLY (5581-5590: telemetry opt-out,
 * privacy gates, crash reporter stubs, product legal/ready flags).
 * Unique gj_crash_reporter_severity_5586 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5586_marker[] = "libcgj-batch5586";

/* Severity floor: max u32 → no practical report admission. */
#define B5586_SEVERITY_FLOOR  0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5586_severity(void)
{
	return B5586_SEVERITY_FLOOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crash_reporter_severity_5586 - report crash severity floor stub.
 *
 * Always returns 0xFFFFFFFF. Integer-only; does not classify stacks
 * or open files. Does not call libc. No parent wires.
 */
uint32_t
gj_crash_reporter_severity_5586(void)
{
	(void)NULL;
	return b5586_severity();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crash_reporter_severity_5586(void)
    __attribute__((alias("gj_crash_reporter_severity_5586")));
