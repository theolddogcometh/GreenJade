/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4646: libcgj product green extract (wave 4650).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4650(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4650 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4650  (alias)
 *   __libcgj_batch4646_marker = "libcgj-batch4646"
 *
 * Milestone 4650 exclusive continuum CREATE-ONLY (4641-4650). Unique
 * gj_libcgj_green_4650 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4600 (batch4596), gj_libcgj_green_4550 (batch4546),
 * gj_libcgj_green_4500 (batch4496), and gj_shell_green_4650
 * (batch4645). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4646_marker[] = "libcgj-batch4646";

/* Product libcgj green lamp for wave 4650. */
#define B4646_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4646_green(void)
{
	return B4646_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4650 - report product libcgj green lamp for wave 4650.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4650(void)
{
	(void)NULL;
	return b4646_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4650(void)
    __attribute__((alias("gj_libcgj_green_4650")));
