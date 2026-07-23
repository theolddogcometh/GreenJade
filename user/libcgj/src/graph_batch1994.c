/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1994: install stage validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_install_stage_ok(unsigned stage);
 *     — Return 1 if stage is in the closed range 0..15 (product install
 *       stage nibble), else 0. Soft pure-data range check only.
 *   int __gj_install_stage_ok  (alias)
 *   __libcgj_batch1994_marker = "libcgj-batch1994"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1994_marker[] = "libcgj-batch1994";

/* Product install stage nibble: 0..15 inclusive. */
#define B1994_STAGE_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if stage is in [0, B1994_STAGE_MAX]. */
static int
b1994_stage_ok(unsigned uStage)
{
	return (uStage <= B1994_STAGE_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_ok — 1 iff stage is in 0..15.
 *
 * stage: product install stage index (unsigned nibble range).
 * Returns 1 when stage <= 15, else 0.
 */
int
gj_install_stage_ok(unsigned uStage)
{
	(void)NULL;
	return b1994_stage_ok(uStage);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_install_stage_ok(unsigned uStage)
    __attribute__((alias("gj_install_stage_ok")));
