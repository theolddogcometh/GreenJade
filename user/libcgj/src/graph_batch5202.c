/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5202: femtoseconds to nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hpet_fs_to_ns_u(uint64_t fs);
 *     - Return floor(fs / 1000000) nanoseconds. 1 ns == 1e6 fs.
 *   uint64_t __gj_hpet_fs_to_ns_u  (alias)
 *   __libcgj_batch5202_marker = "libcgj-batch5202"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_hpet_fs_to_ns_u surface only; no multi-def. Distinct from
 * gj_hpet_period_fs_default_u (batch5201). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5202_marker[] = "libcgj-batch5202";

/* Femtoseconds per nanosecond (1 ns = 1e6 fs). */
#define B5202_FS_PER_NS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5202_fs_to_ns(uint64_t u64Fs)
{
	return u64Fs / B5202_FS_PER_NS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hpet_fs_to_ns_u - convert femtoseconds to nanoseconds.
 *
 * fs: duration in femtoseconds
 *
 * Returns floor(fs / 1000000). Integer-only; no floating point.
 * Self-contained; no parent wires.
 */
uint64_t
gj_hpet_fs_to_ns_u(uint64_t u64Fs)
{
	(void)NULL;
	return b5202_fs_to_ns(u64Fs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hpet_fs_to_ns_u(uint64_t u64Fs)
    __attribute__((alias("gj_hpet_fs_to_ns_u")));
