/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5604: VRAM budget stubs (totals / reserve).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vram_budget_total_mib_5604(void);
 *     - Returns 1024 (soft default VRAM budget total in MiB).
 *   uint32_t gj_vram_budget_reserve_mib_5604(void);
 *     - Returns 128 (soft OS/compositor reserve in MiB).
 *   uint32_t gj_vram_budget_min_mib_5604(void);
 *     - Returns 256 (soft minimum acceptable VRAM budget in MiB).
 *   uint32_t gj_vram_budget_score_5604(void);
 *     - Returns 5604 (wave-tagged VRAM budget score).
 *   uint32_t __gj_vram_budget_total_mib_5604  (alias)
 *   uint32_t __gj_vram_budget_reserve_mib_5604  (alias)
 *   uint32_t __gj_vram_budget_min_mib_5604  (alias)
 *   uint32_t __gj_vram_budget_score_5604  (alias)
 *   __libcgj_batch5604_marker = "libcgj-batch5604"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). Soft VRAM budget stubs
 * for Deck APU product path; no real device probe. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5604_marker[] = "libcgj-batch5604";

#define B5604_TOTAL_MIB    1024u
#define B5604_RESERVE_MIB  128u
#define B5604_MIN_MIB      256u
#define B5604_SCORE        5604u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5604_total(void)
{
	return B5604_TOTAL_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vram_budget_total_mib_5604 - soft default VRAM budget total.
 *
 * Always returns 1024 MiB. Product-path constant; not a hw probe.
 */
uint32_t
gj_vram_budget_total_mib_5604(void)
{
	(void)NULL;
	return b5604_total();
}

/*
 * gj_vram_budget_reserve_mib_5604 - soft OS/compositor reserve.
 *
 * Always returns 128 MiB.
 */
uint32_t
gj_vram_budget_reserve_mib_5604(void)
{
	return B5604_RESERVE_MIB;
}

/*
 * gj_vram_budget_min_mib_5604 - soft minimum acceptable VRAM budget.
 *
 * Always returns 256 MiB.
 */
uint32_t
gj_vram_budget_min_mib_5604(void)
{
	return B5604_MIN_MIB;
}

/*
 * gj_vram_budget_score_5604 - wave-tagged VRAM budget score.
 *
 * Always returns 5604.
 */
uint32_t
gj_vram_budget_score_5604(void)
{
	return B5604_SCORE;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vram_budget_total_mib_5604(void)
    __attribute__((alias("gj_vram_budget_total_mib_5604")));

uint32_t __gj_vram_budget_reserve_mib_5604(void)
    __attribute__((alias("gj_vram_budget_reserve_mib_5604")));

uint32_t __gj_vram_budget_min_mib_5604(void)
    __attribute__((alias("gj_vram_budget_min_mib_5604")));

uint32_t __gj_vram_budget_score_5604(void)
    __attribute__((alias("gj_vram_budget_score_5604")));
