/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7718: getcwd path-max readiness score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gwd_score_7718(void);
 *     - Returns the getcwd path-max readiness score tag for this
 *       continuum (always 7720). Soft compile-time product score.
 *   uint32_t __gj_gwd_score_7718  (alias)
 *   __libcgj_batch7718_marker = "libcgj-batch7718"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Unique gj_gwd_score_7718
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7718_marker[] = "libcgj-batch7718";

/* Wave-end readiness score tag. */
#define B7718_GWD_SCORE  7720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7718_score(void)
{
	return B7718_GWD_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gwd_score_7718 - getcwd path-max readiness score.
 *
 * Always returns 7720. Soft pure-data product score. Does not call
 * getcwd or libc. No parent wires.
 */
uint32_t
gj_gwd_score_7718(void)
{
	(void)NULL;
	return b7718_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gwd_score_7718(void)
    __attribute__((alias("gj_gwd_score_7718")));
