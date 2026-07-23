/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5581: telemetry opt-out flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_telemetry_opt_out_5581(void);
 *     - Returns 1 (telemetry opted out by default). Soft pure-data
 *       product privacy tag; does not talk to any network.
 *   uint32_t __gj_telemetry_opt_out_5581  (alias)
 *   __libcgj_batch5581_marker = "libcgj-batch5581"
 *
 * Exclusive continuum CREATE-ONLY (5581-5590: telemetry opt-out,
 * privacy gates, crash reporter stubs, product legal/ready flags).
 * Unique gj_telemetry_opt_out_5581 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5581_marker[] = "libcgj-batch5581";

/* Default: telemetry opted out (privacy-first). */
#define B5581_TELEMETRY_OPT_OUT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5581_opt_out(void)
{
	return B5581_TELEMETRY_OPT_OUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_telemetry_opt_out_5581 - report telemetry opt-out lamp.
 *
 * Always returns 1 (opted out). Soft pure-data only; does not probe
 * config files or network. Does not call libc. No parent wires.
 */
uint32_t
gj_telemetry_opt_out_5581(void)
{
	(void)NULL;
	return b5581_opt_out();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_telemetry_opt_out_5581(void)
    __attribute__((alias("gj_telemetry_opt_out_5581")));
