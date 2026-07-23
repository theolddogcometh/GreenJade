/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5678: game launch pipeline stage validity.
 *
 * Surface (unique symbols):
 *   int gj_launch_stage_ok_5678(uint32_t stage);
 *     - Return 1 if stage is in the closed range [0, 5] soft launch
 *       pipeline stages (0=idle, 1=appid, 2=depot, 3=mount, 4=spawn,
 *       5=running), else 0. Stub integer gate only.
 *   int __gj_launch_stage_ok_5678  (alias)
 *   __libcgj_batch5678_marker = "libcgj-batch5678"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_launch_stage_ok_5678 surface only; no multi-def. Distinct from
 * process spawn surfaces. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5678_marker[] = "libcgj-batch5678";

#define B5678_STAGE_MAX  5u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5678_stage_ok(uint32_t uStage)
{
	if (uStage > B5678_STAGE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_launch_stage_ok_5678 - soft check launch pipeline stage is valid.
 *
 * stage: soft stage index (0 idle .. 5 running).
 * Returns 1 when stage is in [0, 5], else 0.
 * Does not advance pipeline state. Does not call libc. No parent wires.
 */
int
gj_launch_stage_ok_5678(uint32_t uStage)
{
	(void)NULL;
	return b5678_stage_ok(uStage);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_launch_stage_ok_5678(uint32_t uStage)
    __attribute__((alias("gj_launch_stage_ok_5678")));
