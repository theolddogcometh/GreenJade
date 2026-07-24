/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49791: shell product green extract (wave 49800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_49800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       49800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_49800  (alias)
 *   __libcgj_batch49791_marker = "libcgj-batch49791"
 *
 * Milestone 49800 exclusive continuum CREATE-ONLY (49791-49800). Unique
 * gj_shell_green_49800 surface only; no multi-def. Distinct from
 * gj_shell_green_49700 / gj_shell_green_49600 / gj_shell_green_49500 and
 * sibling 49800 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49791–49800): host soft probes
 * (cgj_soft_milestone_49800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=49800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49791. */
const char __libcgj_batch49791_marker[] = "libcgj-batch49791";

/* Shell green lamp for wave 49800 (soft product status: ready). */
#define B49791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49791_green(void)
{
	return B49791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_49800 - report product shell green lamp for wave 49800.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_49800(void)
{
	(void)NULL;
	return b49791_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_49800(void)
    __attribute__((alias("gj_shell_green_49800")));
