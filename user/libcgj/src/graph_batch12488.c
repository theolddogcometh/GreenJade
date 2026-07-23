/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12488: bar3 input checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_input_check_u_12488(void);
 *     - Returns 0 (soft stub: input checklist slot is soft/unprobed;
 *       not a runtime input-device probe).
 *   uint32_t __gj_bar3_input_check_u_12488  (alias)
 *   __libcgj_batch12488_marker = "libcgj-batch12488"
 *
 * Exclusive continuum CREATE-ONLY (12481-12490: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_input_check_u_12488 surface only; no
 * multi-def. Distinct from gj_bar3_input_check_u_12288,
 * gj_bar3_input_check_u_12088, and sibling check_u soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12488_marker[] = "libcgj-batch12488";

/* Soft-stub input checklist value (unprobed). */
#define B12488_INPUT_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12488_input_check(void)
{
	return B12488_INPUT_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_input_check_u_12488 - soft bar3 input checklist stub.
 *
 * Always returns 0 (soft/unprobed input slot). Does not call libc or
 * probe input devices. No parent wires.
 */
uint32_t
gj_bar3_input_check_u_12488(void)
{
	(void)NULL;
	return b12488_input_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_input_check_u_12488(void)
    __attribute__((alias("gj_bar3_input_check_u_12488")));
