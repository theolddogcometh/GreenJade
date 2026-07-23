/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6517: Steam client deepen score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_deepen_score_6517(uint32_t bin_ok, uint32_t bin_len,
 *                                      uint32_t pkgdir_ok);
 *     - Soft product score in [0, 100] for steam client deepen:
 *         bin_ok contributes 40 when non-zero;
 *         bin_len in [1, 64] contributes up to 40
 *           (scaled: bin_len * 40 / 64, clamped);
 *         pkgdir_ok contributes 20 when non-zero.
 *       Inputs outside domains contribute 0 for that component.
 *   uint32_t __gj_scli_deepen_score_6517  (alias)
 *   __libcgj_batch6517_marker = "libcgj-batch6517"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen). Unique gj_scli_deepen_score_6517 surface only; no multi-def.
 * Distinct from gj_bar3_install_path_deepen_score_6007 and
 * gj_steamui_ready_score_5669. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6517_marker[] = "libcgj-batch6517";

#define B6517_BIN_PTS      40u
#define B6517_LEN_PTS      40u
#define B6517_PKG_PTS      20u
#define B6517_LEN_MIN       1u
#define B6517_LEN_MAX      64u
#define B6517_SCORE_MAX   100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6517_score(uint32_t uBinOk, uint32_t uBinLen, uint32_t uPkgdirOk)
{
	uint32_t uScore;
	uint32_t uLenPts;
	uint32_t uClampedLen;

	uScore = 0u;
	if (uBinOk != 0u) {
		uScore += B6517_BIN_PTS;
	}

	if (uBinLen >= B6517_LEN_MIN && uBinLen <= B6517_LEN_MAX) {
		uClampedLen = uBinLen;
		uLenPts = (uClampedLen * B6517_LEN_PTS) / B6517_LEN_MAX;
		if (uLenPts > B6517_LEN_PTS) {
			uLenPts = B6517_LEN_PTS;
		}
		uScore += uLenPts;
	}

	if (uPkgdirOk != 0u) {
		uScore += B6517_PKG_PTS;
	}

	if (uScore > B6517_SCORE_MAX) {
		return B6517_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_deepen_score_6517 - soft steam client deepen score [0, 100].
 *
 * bin_ok:    non-zero when client binary basename shape is ok
 * bin_len:   measured binary basename length
 * pkgdir_ok: non-zero when package-dir basename shape is ok
 *
 * Returns weighted soft score capped at 100. Pure integer; no libc.
 * No parent wires.
 */
uint32_t
gj_scli_deepen_score_6517(uint32_t uBinOk, uint32_t uBinLen,
    uint32_t uPkgdirOk)
{
	(void)NULL;
	return b6517_score(uBinOk, uBinLen, uPkgdirOk);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_deepen_score_6517(uint32_t uBinOk, uint32_t uBinLen,
    uint32_t uPkgdirOk)
    __attribute__((alias("gj_scli_deepen_score_6517")));
