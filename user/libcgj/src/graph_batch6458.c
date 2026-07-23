/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6458: achievement progress soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_progress_ok_6458(uint32_t progress, uint32_t goal);
 *     - Soft progress gate: return 1 when goal != 0 and progress >= goal,
 *       else 0. Used before setting an unlock bit.
 *   uint32_t __gj_ach_progress_ok_6458  (alias)
 *   __libcgj_batch6458_marker = "libcgj-batch6458"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_progress_ok_6458 surface only; no multi-def.
 * Distinct from bit/mask helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6458_marker[] = "libcgj-batch6458";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6458_progress_ok(uint32_t uProgress, uint32_t uGoal)
{
	if (uGoal == 0u) {
		return 0u;
	}
	if (uProgress < uGoal) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_progress_ok_6458 - soft achievement progress threshold gate.
 *
 * progress: current counter
 * goal:     required non-zero threshold
 *
 * Returns 1 when progress meets goal, else 0. Pure integer.
 * No parent wires.
 */
uint32_t
gj_ach_progress_ok_6458(uint32_t progress, uint32_t goal)
{
	(void)NULL;
	return b6458_progress_ok(progress, goal);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_progress_ok_6458(uint32_t progress, uint32_t goal)
    __attribute__((alias("gj_ach_progress_ok_6458")));
