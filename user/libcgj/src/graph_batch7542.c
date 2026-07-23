/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7542: sigaction flag value table stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigaction_flag_value_7542(uint32_t flag_id);
 *     - Map sigaction flag id (0..6) to Linux x86_64 SA_* bit value.
 *       Unknown flag_id → 0.
 *   uint32_t gj_sigaction_flag_count_7542(void);
 *     - Number of known sa_flags bits in the table (always 7).
 *   uint32_t __gj_sigaction_flag_value_7542  (alias)
 *   uint32_t __gj_sigaction_flag_count_7542  (alias)
 *   __libcgj_batch7542_marker = "libcgj-batch7542"
 *
 * Exclusive continuum CREATE-ONLY (7541-7550: sigaction flag stubs).
 * Unique *_7542 surfaces only; no multi-def. Soft pure-data SA_* table
 * only; does not install handlers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7542_marker[] = "libcgj-batch7542";

#define B7542_FLAG_MAX  7u

/*
 * Linux x86_64 sa_flags values (local freestanding copies):
 *   0 SA_NOCLDSTOP 0x00000001
 *   1 SA_NOCLDWAIT 0x00000002
 *   2 SA_SIGINFO   0x00000004
 *   3 SA_ONSTACK   0x08000000
 *   4 SA_RESTART   0x10000000
 *   5 SA_NODEFER   0x40000000
 *   6 SA_RESETHAND 0x80000000
 */
static const uint32_t s_b7542_flag[B7542_FLAG_MAX] = {
	0x00000001u, /* 0 SA_NOCLDSTOP */
	0x00000002u, /* 1 SA_NOCLDWAIT */
	0x00000004u, /* 2 SA_SIGINFO   */
	0x08000000u, /* 3 SA_ONSTACK   */
	0x10000000u, /* 4 SA_RESTART   */
	0x40000000u, /* 5 SA_NODEFER   */
	0x80000000u  /* 6 SA_RESETHAND */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7542_value(uint32_t u32Flag)
{
	if (u32Flag >= B7542_FLAG_MAX) {
		return 0u;
	}
	return s_b7542_flag[u32Flag];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigaction_flag_value_7542 - map flag id to SA_* bit.
 *
 * flag_id: 0..6 known; else 0. Soft table only; does not call sigaction.
 * No parent wires.
 */
uint32_t
gj_sigaction_flag_value_7542(uint32_t flag_id)
{
	(void)NULL;
	return b7542_value(flag_id);
}

/*
 * gj_sigaction_flag_count_7542 - known SA flag count.
 *
 * Always returns 7.
 */
uint32_t
gj_sigaction_flag_count_7542(void)
{
	return B7542_FLAG_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sigaction_flag_value_7542(uint32_t flag_id)
    __attribute__((alias("gj_sigaction_flag_value_7542")));

uint32_t __gj_sigaction_flag_count_7542(void)
    __attribute__((alias("gj_sigaction_flag_count_7542")));
