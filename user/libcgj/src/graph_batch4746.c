/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4746: libcgj product green extract (wave 4750).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4750(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4750 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4750  (alias)
 *   __libcgj_batch4746_marker = "libcgj-batch4746"
 *
 * Milestone 4750 exclusive continuum CREATE-ONLY (4741-4750). Unique
 * gj_libcgj_green_4750 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4700 (batch4696), gj_libcgj_green_4650 (batch4646),
 * gj_libcgj_green_4600 (batch4596), and gj_shell_green_4750
 * (batch4745). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4746_marker[] = "libcgj-batch4746";

/* Product libcgj green lamp for wave 4750. */
#define B4746_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4746_green(void)
{
	return B4746_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4750 - report product libcgj green lamp for wave 4750.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4750(void)
{
	(void)NULL;
	return b4746_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4750(void)
    __attribute__((alias("gj_libcgj_green_4750")));
