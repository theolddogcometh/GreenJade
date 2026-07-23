/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2362: product install stage done predicate
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_install_stage_is_done(unsigned stage);
 *     - Return 1 if stage is at or past the product install stage
 *       nibble max (15), else 0. Soft pure-data gate only.
 *   int __gj_install_stage_is_done  (alias)
 *   __libcgj_batch2362_marker = "libcgj-batch2362"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_install_stage_ok (batch1994) and gj_install_stage_advance
 * (batch2361) - unique is_done surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2362_marker[] = "libcgj-batch2362";

/* Product install stage nibble max: 0..15 inclusive (batch1994). */
#define B2362_STAGE_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if stage has reached the terminal product install stage. */
static int
b2362_is_done(unsigned uStage)
{
	if (uStage >= B2362_STAGE_MAX) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_is_done - 1 iff install stage is finished.
 *
 * stage: current install stage index
 *
 * Returns 1 when stage >= 15, else 0. Does not call libc.
 */
int
gj_install_stage_is_done(unsigned uStage)
{
	(void)NULL;
	return b2362_is_done(uStage);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_install_stage_is_done(unsigned uStage)
    __attribute__((alias("gj_install_stage_is_done")));
