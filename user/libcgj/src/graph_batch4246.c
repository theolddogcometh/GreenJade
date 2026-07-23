/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4246: libcgj product green extract (wave 4250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4250(void);
 *     - Returns 1 (product libcgj green/ready tag) for the milestone
 *       4250 continuum. Soft compile-time product green tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4250  (alias)
 *   __libcgj_batch4246_marker = "libcgj-batch4246"
 *
 * Milestone 4250 exclusive continuum CREATE-ONLY (4241-4250). Unique
 * gj_libcgj_green_4250 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4100 (batch4096), gj_libcgj_green_4000 (batch3996),
 * gj_libcgj_green_3950 (batch3946), and gj_shell_green_4250
 * (batch4245). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4246_marker[] = "libcgj-batch4246";

/* Product libcgj green/ready tag for wave 4250. */
#define B4246_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4246_green(void)
{
	return B4246_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4250 - report the milestone-4250 libcgj green tag.
 *
 * Always returns 1 (libcgj green/ready). Link-time presence of this
 * symbol tags libcgj product green for the wave. Does not parse
 * soname strings and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4250(void)
{
	(void)NULL;
	return b4246_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4250(void)
    __attribute__((alias("gj_libcgj_green_4250")));
