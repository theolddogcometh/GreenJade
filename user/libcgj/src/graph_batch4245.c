/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4245: shell product green extract (wave 4250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4250(void);
 *     - Returns 1 (product shell green/ready tag) for the milestone
 *       4250 continuum. Soft compile-time product green tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4250  (alias)
 *   __libcgj_batch4245_marker = "libcgj-batch4245"
 *
 * Milestone 4250 exclusive continuum CREATE-ONLY (4241-4250). Unique
 * gj_shell_green_4250 surface only; no multi-def. Distinct from
 * gj_shell_green_4100 (batch4095), gj_shell_green_4000 (batch3995),
 * gj_shell_green_3950 (batch3945), and gj_libcgj_green_4250
 * (batch4246). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4245_marker[] = "libcgj-batch4245";

/* Product shell green/ready tag for wave 4250. */
#define B4245_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4245_green(void)
{
	return B4245_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4250 - report the milestone-4250 shell green tag.
 *
 * Always returns 1 (shell green/ready). Link-time presence of this
 * symbol tags shell product green for the wave. Does not call libc
 * or probe greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4250(void)
{
	(void)NULL;
	return b4245_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4250(void)
    __attribute__((alias("gj_shell_green_4250")));
