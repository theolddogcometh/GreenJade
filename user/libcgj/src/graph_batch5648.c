/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5648: installer bootloader progress (0-100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_bootloader_progress_5648(uint32_t step, uint32_t steps);
 *     - Return percent complete (0..100) for bootloader install steps.
 *       steps==0 → 0; step>=steps → 100; else (step*100)/steps.
 *   uint32_t gj_install_bootloader_step_ok_5648(uint32_t step, uint32_t steps);
 *     - Return 1 if step < steps (valid in-progress index), else 0.
 *   uint32_t __gj_install_bootloader_progress_5648  (alias)
 *   uint32_t __gj_install_bootloader_step_ok_5648  (alias)
 *   __libcgj_batch5648_marker = "libcgj-batch5648"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5648_marker[] = "libcgj-batch5648";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5648_clamp_pct(uint32_t u32Pct)
{
	if (u32Pct > 100u) {
		return 100u;
	}
	return u32Pct;
}

static uint32_t
b5648_progress(uint32_t u32Step, uint32_t u32Steps)
{
	uint32_t u32Pct;

	if (u32Steps == 0u) {
		return 0u;
	}
	if (u32Step >= u32Steps) {
		return 100u;
	}
	if (u32Step > (UINT32_MAX / 100u)) {
		u32Pct = u32Step / (u32Steps / 100u);
	} else {
		u32Pct = (u32Step * 100u) / u32Steps;
	}
	return b5648_clamp_pct(u32Pct);
}

static uint32_t
b5648_step_ok(uint32_t u32Step, uint32_t u32Steps)
{
	if (u32Steps == 0u) {
		return 0u;
	}
	return (u32Step < u32Steps) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_bootloader_progress_5648 - bootloader stage percent complete.
 *
 * step:  completed install steps (copy loader, set BootOrder, ...)
 * steps: total steps (0 → 0)
 *
 * Returns 0..100. Pure integer; no EFI variables. No parent wires.
 */
uint32_t
gj_install_bootloader_progress_5648(uint32_t u32Step, uint32_t u32Steps)
{
	(void)NULL;
	return b5648_progress(u32Step, u32Steps);
}

/*
 * gj_install_bootloader_step_ok_5648 - true when step index is in range.
 *
 * step:  zero-based step index
 * steps: total steps
 *
 * Returns 1 if step < steps and steps != 0, else 0.
 */
uint32_t
gj_install_bootloader_step_ok_5648(uint32_t u32Step, uint32_t u32Steps)
{
	return b5648_step_ok(u32Step, u32Steps);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_bootloader_progress_5648(uint32_t u32Step, uint32_t u32Steps)
    __attribute__((alias("gj_install_bootloader_progress_5648")));

uint32_t __gj_install_bootloader_step_ok_5648(uint32_t u32Step, uint32_t u32Steps)
    __attribute__((alias("gj_install_bootloader_step_ok_5648")));
