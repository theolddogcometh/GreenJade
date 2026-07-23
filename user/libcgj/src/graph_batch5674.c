/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5674: game launch depot mount ready score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_depot_mount_score_5674(uint32_t depot_id, uint32_t flags);
 *     - Stub mount readiness score in [0, 100]. Invalid depot_id → 0;
 *       else base 50 plus 10 per set bit among low three mount flags
 *       (bit0=content, bit1=shared, bit2=dlc), clamped to 100.
 *   uint32_t __gj_depot_mount_score_5674  (alias)
 *   __libcgj_batch5674_marker = "libcgj-batch5674"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_depot_mount_score_5674 surface only; no multi-def. Distinct from
 * gj_depot_id_validate_5673. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5674_marker[] = "libcgj-batch5674";

#define B5674_DEPOT_BAD   0xFFFFFFFFu
#define B5674_SCORE_BASE  50u
#define B5674_SCORE_STEP  10u
#define B5674_SCORE_MAX   100u
#define B5674_MOUNT_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5674_pop3(uint32_t uFlags)
{
	uint32_t uN;
	uint32_t uBits;

	uN = 0u;
	uBits = uFlags & B5674_MOUNT_MASK;
	while (uBits != 0u) {
		uN += (uBits & 1u);
		uBits >>= 1;
	}
	return uN;
}

static uint32_t
b5674_mount_score(uint32_t uDepotId, uint32_t uFlags)
{
	uint32_t uScore;

	if (uDepotId == 0u) {
		return 0u;
	}
	if (uDepotId == B5674_DEPOT_BAD) {
		return 0u;
	}
	uScore = B5674_SCORE_BASE + (B5674_SCORE_STEP * b5674_pop3(uFlags));
	if (uScore > B5674_SCORE_MAX) {
		return B5674_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_mount_score_5674 - stub depot mount readiness score.
 *
 * depot_id: Steam depot identifier (must be nonzero / non-sentinel).
 * flags: soft mount role bits (bit0 content, bit1 shared, bit2 dlc).
 * Returns 0 if depot_id invalid; else score in [50, 80] typically,
 * clamped to 100. Does not mount filesystems. No parent wires.
 */
uint32_t
gj_depot_mount_score_5674(uint32_t uDepotId, uint32_t uFlags)
{
	(void)NULL;
	return b5674_mount_score(uDepotId, uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_depot_mount_score_5674(uint32_t uDepotId, uint32_t uFlags)
    __attribute__((alias("gj_depot_mount_score_5674")));
