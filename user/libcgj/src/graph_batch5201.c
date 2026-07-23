/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5201: default HPET period in femtoseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hpet_period_fs_default_u(void);
 *     - Return the conventional mainboard HPET/legacy timer period in
 *       femtoseconds for ~14.31818 MHz (69841279 fs per tick).
 *   uint64_t __gj_hpet_period_fs_default_u  (alias)
 *   __libcgj_batch5201_marker = "libcgj-batch5201"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique —
 * hpet_period_fs_default_u, hpet_fs_to_ns_u, hpet_ns_to_ticks_u,
 * hpet_ticks_to_ns_u, timer_hz_to_period_ns_u, timer_period_ns_to_hz_u,
 * timer_deadline_u, timer_overdue_u, timer_slack_u, batch_id_5210).
 * Unique gj_hpet_period_fs_default_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5201_marker[] = "libcgj-batch5201";

/*
 * ~14.31818 MHz crystal period in femtoseconds:
 * floor(1e15 / 14318180 + 0.5) == 69841279.
 */
#define B5201_HPET_PERIOD_FS  69841279ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5201_period_fs(void)
{
	return B5201_HPET_PERIOD_FS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hpet_period_fs_default_u - default HPET tick period in femtoseconds.
 *
 * Always returns 69841279 (approx 14.318 MHz period). Soft compile-time
 * constant for HPET period math; does not probe hardware. No parent wires.
 */
uint64_t
gj_hpet_period_fs_default_u(void)
{
	(void)NULL;
	return b5201_period_fs();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hpet_period_fs_default_u(void)
    __attribute__((alias("gj_hpet_period_fs_default_u")));
