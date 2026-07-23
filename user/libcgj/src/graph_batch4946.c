/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4946: libcgj product green extract (wave 4950).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4950(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4950 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4950  (alias)
 *   __libcgj_batch4946_marker = "libcgj-batch4946"
 *
 * Milestone 4950 exclusive continuum CREATE-ONLY (4941-4950). Unique
 * gj_libcgj_green_4950 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4900 (batch4896), gj_libcgj_green_4850 (batch4846),
 * gj_libcgj_green_4800 (batch4796), and gj_shell_green_4950
 * (batch4945). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4946_marker[] = "libcgj-batch4946";

/* Product libcgj green lamp for wave 4950. */
#define B4946_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4946_green(void)
{
	return B4946_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4950 - report product libcgj green lamp for wave 4950.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4950(void)
{
	(void)NULL;
	return b4946_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4950(void)
    __attribute__((alias("gj_libcgj_green_4950")));
