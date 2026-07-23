/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5672: game launch appid validate score stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_appid_validate_score_5672(uint32_t appid);
 *     - Stub validate score in [0, 100]. appid 0 or 0xFFFFFFFF → 0;
 *       else a soft score from low 7 bits of appid clamped to [1, 100].
 *   uint32_t __gj_appid_validate_score_5672  (alias)
 *   __libcgj_batch5672_marker = "libcgj-batch5672"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_appid_validate_score_5672 surface only; no multi-def. Distinct
 * from gj_appid_validate_5671. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5672_marker[] = "libcgj-batch5672";

#define B5672_APPID_BAD  0xFFFFFFFFu
#define B5672_SCORE_MAX  100u
#define B5672_SCORE_MASK 0x7Fu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Stub score: invalid appid → 0; else (appid & 0x7F) with 0 mapped to
 * 1, clamped to 100. Pure arithmetic; no catalog lookup.
 */
static uint32_t
b5672_score(uint32_t uAppid)
{
	uint32_t uScore;

	if (uAppid == 0u) {
		return 0u;
	}
	if (uAppid == B5672_APPID_BAD) {
		return 0u;
	}
	uScore = uAppid & B5672_SCORE_MASK;
	if (uScore == 0u) {
		uScore = 1u;
	}
	if (uScore > B5672_SCORE_MAX) {
		return B5672_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_appid_validate_score_5672 - stub appid validate score for launch.
 *
 * appid: Steam AppID as an unsigned 32-bit value.
 * Returns 0 if appid is 0 or 0xFFFFFFFF; else a soft score in [1, 100].
 * Does not call libc. No parent wires.
 */
uint32_t
gj_appid_validate_score_5672(uint32_t uAppid)
{
	(void)NULL;
	return b5672_score(uAppid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_appid_validate_score_5672(uint32_t uAppid)
    __attribute__((alias("gj_appid_validate_score_5672")));
