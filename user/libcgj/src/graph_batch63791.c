/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63791: shell product green extract (wave 63800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_63800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       63800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_63800  (alias)
 *   __libcgj_batch63791_marker = "libcgj-batch63791"
 *
 * Milestone 63800 exclusive continuum CREATE-ONLY (63791-63800). Unique
 * gj_shell_green_63800 surface only; no multi-def. Distinct from
 * gj_shell_green_25800 / gj_shell_green_25700 / gj_shell_green_25600 and
 * sibling 63800 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (63791–63800): host soft probes
 * (cgj_soft_milestone_63800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=63800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 63791. */
const char __libcgj_batch63791_marker[] = "libcgj-batch63791";

/* Shell green lamp for wave 63800 (soft product status: ready). */
#define B63791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b63791_green(void)
{
	return B63791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_63800 - report product shell green lamp for wave 63800.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_63800(void)
{
	(void)NULL;
	return b63791_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_63800(void)
    __attribute__((alias("gj_shell_green_63800")));
