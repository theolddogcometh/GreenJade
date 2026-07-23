/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5587: crash report sink mode stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crash_report_sink_mode_5587(void);
 *     - Returns 0 (sink mode = drop / discard). Integer-only stub;
 *       no upload, no local spool.
 *   uint32_t __gj_crash_report_sink_mode_5587  (alias)
 *   __libcgj_batch5587_marker = "libcgj-batch5587"
 *
 * Exclusive continuum CREATE-ONLY (5581-5590: telemetry opt-out,
 * privacy gates, crash reporter stubs, product legal/ready flags).
 * Unique gj_crash_report_sink_mode_5587 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5587_marker[] = "libcgj-batch5587";

/* Sink mode: 0 = drop (no spool, no upload). */
#define B5587_SINK_MODE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5587_sink_mode(void)
{
	return B5587_SINK_MODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crash_report_sink_mode_5587 - report crash-report sink mode stub.
 *
 * Always returns 0 (drop). Integer-only; does not open sockets or
 * create spool dirs. Does not call libc. No parent wires.
 */
uint32_t
gj_crash_report_sink_mode_5587(void)
{
	(void)NULL;
	return b5587_sink_mode();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crash_report_sink_mode_5587(void)
    __attribute__((alias("gj_crash_report_sink_mode_5587")));
