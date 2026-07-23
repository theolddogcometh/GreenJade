/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1918: hat switch up-direction predicate.
 *
 * Surface (unique symbols):
 *   int gj_hat_is_up(int hat);
 *     — Return 1 if the up bit (0x01) is set in hat, else 0.
 *       Encoding matches common gamepad hat bitmasks:
 *         0x00 center, 0x01 up, 0x02 right, 0x04 down, 0x08 left
 *         (diagonals OR the up bit and still count as up).
 *   int __gj_hat_is_up  (alias)
 *   __libcgj_batch1918_marker = "libcgj-batch1918"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1918_marker[] = "libcgj-batch1918";

/* Up direction bit in hat mask. */
#define B1918_HAT_UP  0x01

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if up bit is set. */
static int
b1918_is_up(int nHat)
{
	return ((nHat & B1918_HAT_UP) != 0) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hat_is_up — 1 if hat has the up bit (0x01) set.
 *
 * hat: hat bit-mask (0 center; 0x01 up; diagonals OR bits)
 * Returns 1 when up is active (including up-diagonals), else 0.
 */
int
gj_hat_is_up(int nHat)
{
	(void)NULL;
	return b1918_is_up(nHat);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hat_is_up(int nHat)
    __attribute__((alias("gj_hat_is_up")));
