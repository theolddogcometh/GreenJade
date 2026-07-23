/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6355: Xbox right-thumbstick axial deadzone stub.
 *
 * Surface (unique symbols):
 *   int32_t gj_xpad_rstick_dz_6355(int32_t axis, uint32_t deadzone);
 *     - Axial deadzone for one right-stick axis (X or Y). If |axis| is
 *       less than or equal to deadzone, return 0; else return axis
 *       unchanged. Parallel surface to gj_xpad_lstick_dz_6354 for the
 *       right stick path.
 *   int32_t __gj_xpad_rstick_dz_6355  (alias)
 *   __libcgj_batch6355_marker = "libcgj-batch6355"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs). Unique gj_xpad_rstick_dz_6355 surface only; no multi-def.
 * Distinct from gj_xpad_lstick_dz_6354. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6355_marker[] = "libcgj-batch6355";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6355_abs_i32(int32_t iV)
{
	if (iV < 0) {
		return (uint32_t)(-(iV + 1)) + 1u;
	}
	return (uint32_t)iV;
}

static int32_t
b6355_dz(int32_t iAxis, uint32_t uDz)
{
	if (b6355_abs_i32(iAxis) <= uDz) {
		return 0;
	}
	return iAxis;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_rstick_dz_6355 - right stick single-axis axial deadzone.
 *
 * Soft zeroing inside the deadzone radius along one axis. Pure integer;
 * no float. Does not call libc. No parent wires.
 */
int32_t
gj_xpad_rstick_dz_6355(int32_t iAxis, uint32_t uDeadzone)
{
	(void)NULL;
	return b6355_dz(iAxis, uDeadzone);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_xpad_rstick_dz_6355(int32_t iAxis, uint32_t uDeadzone)
    __attribute__((alias("gj_xpad_rstick_dz_6355")));
