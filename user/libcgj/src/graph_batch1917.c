/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1917: hat switch centered predicate.
 *
 * Surface (unique symbols):
 *   int gj_hat_is_centered(int hat);
 *     — Return 1 if hat is the centered bit-mask value (0), else 0.
 *       Encoding matches common gamepad hat bitmasks:
 *         0x00 center, 0x01 up, 0x02 right, 0x04 down, 0x08 left
 *         (diagonals are OR combinations).
 *   int __gj_hat_is_centered  (alias)
 *   __libcgj_batch1917_marker = "libcgj-batch1917"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1917_marker[] = "libcgj-batch1917";

/* Centered hat mask. */
#define B1917_HAT_CENTER  0

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if hat is centered (0). */
static int
b1917_is_center(int nHat)
{
	return (nHat == B1917_HAT_CENTER) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hat_is_centered — 1 if hat bitmask is 0 (center).
 *
 * hat: hat bit-mask (0 center; direction bits set when active)
 * Returns 1 when centered, else 0.
 */
int
gj_hat_is_centered(int nHat)
{
	(void)NULL;
	return b1917_is_center(nHat);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hat_is_centered(int nHat)
    __attribute__((alias("gj_hat_is_centered")));
