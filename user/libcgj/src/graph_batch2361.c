/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2361: product install stage advance
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_install_stage_advance(unsigned stage);
 *     - Return stage+1 when stage < 15, else 15 (clamp at the product
 *       install stage nibble max used by gj_install_stage_ok). Soft
 *       pure-data step; does not mutate caller storage.
 *   unsigned __gj_install_stage_advance  (alias)
 *   __libcgj_batch2361_marker = "libcgj-batch2361"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_install_stage_ok (batch1994) - unique advance surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2361_marker[] = "libcgj-batch2361";

/* Product install stage nibble max: 0..15 inclusive (batch1994). */
#define B2361_STAGE_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

/* Next stage, clamped at B2361_STAGE_MAX. */
static unsigned
b2361_advance(unsigned uStage)
{
	if (uStage >= B2361_STAGE_MAX) {
		return B2361_STAGE_MAX;
	}
	return uStage + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_advance - step product install stage forward by one.
 *
 * stage: current install stage index
 *
 * Returns stage+1 when stage < 15, else 15. Does not call libc.
 */
unsigned
gj_install_stage_advance(unsigned uStage)
{
	(void)NULL;
	return b2361_advance(uStage);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_install_stage_advance(unsigned uStage)
    __attribute__((alias("gj_install_stage_advance")));
