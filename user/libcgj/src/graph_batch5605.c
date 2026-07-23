/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5605: VRAM budget stubs (usable / ok).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vram_budget_usable_mib_5605(uint32_t total, uint32_t reserve);
 *     - Return total - reserve when total >= reserve, else 0.
 *   uint32_t gj_vram_budget_ok_5605(uint32_t usable, uint32_t min_mib);
 *     - Return 1 when usable >= min_mib, else 0.
 *   uint32_t gj_vram_budget_default_usable_5605(void);
 *     - Returns 896 (soft 1024 - 128 default usable MiB).
 *   uint32_t gj_vram_budget_ready_5605(void);
 *     - Returns 1 (soft VRAM budget path ready lamp).
 *   uint32_t __gj_vram_budget_usable_mib_5605  (alias)
 *   uint32_t __gj_vram_budget_ok_5605  (alias)
 *   uint32_t __gj_vram_budget_default_usable_5605  (alias)
 *   uint32_t __gj_vram_budget_ready_5605  (alias)
 *   __libcgj_batch5605_marker = "libcgj-batch5605"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). VRAM budget arithmetic
 * stubs; distinct from gj_vram_budget_*_5604. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5605_marker[] = "libcgj-batch5605";

#define B5605_DEFAULT_USABLE  896u
#define B5605_READY           1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5605_usable(uint32_t u32Total, uint32_t u32Reserve)
{
	if (u32Total < u32Reserve) {
		return 0u;
	}
	return u32Total - u32Reserve;
}

static uint32_t
b5605_ok(uint32_t u32Usable, uint32_t u32Min)
{
	return (u32Usable >= u32Min) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vram_budget_usable_mib_5605 - soft usable VRAM after reserve.
 *
 * total: budget total in MiB
 * reserve: reserved MiB
 *
 * Returns total - reserve when total >= reserve, else 0.
 */
uint32_t
gj_vram_budget_usable_mib_5605(uint32_t u32Total, uint32_t u32Reserve)
{
	(void)NULL;
	return b5605_usable(u32Total, u32Reserve);
}

/*
 * gj_vram_budget_ok_5605 - soft check usable against minimum.
 *
 * usable: usable MiB
 * min_mib: minimum required MiB
 *
 * Returns 1 when usable >= min_mib, else 0.
 */
uint32_t
gj_vram_budget_ok_5605(uint32_t u32Usable, uint32_t u32MinMib)
{
	return b5605_ok(u32Usable, u32MinMib);
}

/*
 * gj_vram_budget_default_usable_5605 - soft default usable budget.
 *
 * Always returns 896 (1024 - 128 product-path default).
 */
uint32_t
gj_vram_budget_default_usable_5605(void)
{
	return B5605_DEFAULT_USABLE;
}

/*
 * gj_vram_budget_ready_5605 - soft VRAM budget path ready lamp.
 *
 * Always returns 1. Compile-time readiness tag only.
 */
uint32_t
gj_vram_budget_ready_5605(void)
{
	return B5605_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vram_budget_usable_mib_5605(uint32_t u32Total, uint32_t u32Reserve)
    __attribute__((alias("gj_vram_budget_usable_mib_5605")));

uint32_t __gj_vram_budget_ok_5605(uint32_t u32Usable, uint32_t u32MinMib)
    __attribute__((alias("gj_vram_budget_ok_5605")));

uint32_t __gj_vram_budget_default_usable_5605(void)
    __attribute__((alias("gj_vram_budget_default_usable_5605")));

uint32_t __gj_vram_budget_ready_5605(void)
    __attribute__((alias("gj_vram_budget_ready_5605")));
