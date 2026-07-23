/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5676: game launch process spawn score stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_process_spawn_score_5676(uint32_t argc, uint32_t flags);
 *     - Stub process-spawn readiness score in [0, 100]. argc 0 → 0;
 *       else base min(argc, 8) * 8 plus 10 per set bit among low three
 *       spawn flags (bit0=env, bit1=cwd, bit2=fd), clamped to 100.
 *   uint32_t __gj_process_spawn_score_5676  (alias)
 *   __libcgj_batch5676_marker = "libcgj-batch5676"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_process_spawn_score_5676 surface only; no multi-def. Distinct
 * from depot mount surfaces. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5676_marker[] = "libcgj-batch5676";

#define B5676_ARGC_CAP    8u
#define B5676_ARGC_STEP   8u
#define B5676_FLAG_STEP   10u
#define B5676_SCORE_MAX   100u
#define B5676_SPAWN_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5676_pop3(uint32_t uFlags)
{
	uint32_t uN;
	uint32_t uBits;

	uN = 0u;
	uBits = uFlags & B5676_SPAWN_MASK;
	while (uBits != 0u) {
		uN += (uBits & 1u);
		uBits >>= 1;
	}
	return uN;
}

static uint32_t
b5676_spawn_score(uint32_t uArgc, uint32_t uFlags)
{
	uint32_t uN;
	uint32_t uScore;

	if (uArgc == 0u) {
		return 0u;
	}
	uN = uArgc;
	if (uN > B5676_ARGC_CAP) {
		uN = B5676_ARGC_CAP;
	}
	uScore = (uN * B5676_ARGC_STEP) + (B5676_FLAG_STEP * b5676_pop3(uFlags));
	if (uScore > B5676_SCORE_MAX) {
		return B5676_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_process_spawn_score_5676 - stub process spawn readiness score.
 *
 * argc: soft argument count for the launch (0 = nothing to spawn).
 * flags: soft spawn context bits (bit0 env, bit1 cwd, bit2 fd).
 * Returns 0 if argc is 0; else a stub score in [8, 100].
 * Does not spawn processes. Does not call libc. No parent wires.
 */
uint32_t
gj_process_spawn_score_5676(uint32_t uArgc, uint32_t uFlags)
{
	(void)NULL;
	return b5676_spawn_score(uArgc, uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_process_spawn_score_5676(uint32_t uArgc, uint32_t uFlags)
    __attribute__((alias("gj_process_spawn_score_5676")));
