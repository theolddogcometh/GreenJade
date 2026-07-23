/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5677: game launch process spawn args predicate.
 *
 * Surface (unique symbols):
 *   int gj_process_spawn_args_ok_5677(uint32_t argc, uint32_t flags);
 *     - Return 1 if argc is in [1, 64] and flags has required spawn arm
 *       bit0 (argv/env ready), else 0. Soft pure-data gate only.
 *   int __gj_process_spawn_args_ok_5677  (alias)
 *   __libcgj_batch5677_marker = "libcgj-batch5677"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_process_spawn_args_ok_5677 surface only; no multi-def. Distinct
 * from gj_process_spawn_score_5676. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5677_marker[] = "libcgj-batch5677";

#define B5677_ARGC_MIN    1u
#define B5677_ARGC_MAX    64u
#define B5677_SPAWN_ARM   0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5677_args_ok(uint32_t uArgc, uint32_t uFlags)
{
	if (uArgc < B5677_ARGC_MIN) {
		return 0;
	}
	if (uArgc > B5677_ARGC_MAX) {
		return 0;
	}
	if ((uFlags & B5677_SPAWN_ARM) != B5677_SPAWN_ARM) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_process_spawn_args_ok_5677 - soft check spawn argc/flags are ready.
 *
 * argc: soft argument count for the launch process.
 * flags: soft spawn context bits (bit0 required = argv/env ready).
 * Returns 1 when argc in [1, 64] and bit0 is set, else 0.
 * Does not spawn processes. Does not call libc. No parent wires.
 */
int
gj_process_spawn_args_ok_5677(uint32_t uArgc, uint32_t uFlags)
{
	(void)NULL;
	return b5677_args_ok(uArgc, uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_process_spawn_args_ok_5677(uint32_t uArgc, uint32_t uFlags)
    __attribute__((alias("gj_process_spawn_args_ok_5677")));
