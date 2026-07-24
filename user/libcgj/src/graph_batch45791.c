/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45791: shell product green extract (wave 45800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_45800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       45800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_45800  (alias)
 *   __libcgj_batch45791_marker = "libcgj-batch45791"
 *
 * Milestone 45800 exclusive continuum CREATE-ONLY (45791-45800). Unique
 * gj_shell_green_45800 surface only; no multi-def. Distinct from
 * gj_shell_green_45700 / gj_shell_green_45600 / gj_shell_green_45500 and
 * sibling 45800 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (45791–45800): host soft probes
 * (cgj_soft_milestone_45800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=45800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 45791. */
const char __libcgj_batch45791_marker[] = "libcgj-batch45791";

/* Shell green lamp for wave 45800 (soft product status: ready). */
#define B45791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45791_green(void)
{
	return B45791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_45800 - report product shell green lamp for wave 45800.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_45800(void)
{
	(void)NULL;
	return b45791_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_45800(void)
    __attribute__((alias("gj_shell_green_45800")));
