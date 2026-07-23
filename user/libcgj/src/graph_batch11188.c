/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11188: bar3 input checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_input_check_u_11188(void);
 *     - Returns 0 (soft stub: input checklist slot is soft/unprobed;
 *       not a runtime input-device probe).
 *   uint32_t __gj_bar3_input_check_u_11188  (alias)
 *   __libcgj_batch11188_marker = "libcgj-batch11188"
 *
 * Exclusive continuum CREATE-ONLY (11181-11190: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_input_check_u_11188 surface only; no
 * multi-def. Distinct from gj_bar3_input_check_u_10988 and sibling
 * check_u soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11188_marker[] = "libcgj-batch11188";

/* Soft-stub input checklist value (unprobed). */
#define B11188_INPUT_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11188_input_check(void)
{
	return B11188_INPUT_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_input_check_u_11188 - soft bar3 input checklist stub.
 *
 * Always returns 0 (soft/unprobed input slot). Does not call libc or
 * probe input devices. No parent wires.
 */
uint32_t
gj_bar3_input_check_u_11188(void)
{
	(void)NULL;
	return b11188_input_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_input_check_u_11188(void)
    __attribute__((alias("gj_bar3_input_check_u_11188")));
