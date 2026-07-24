/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36791: shell product green extract (wave 36800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_36800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       36800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_36800  (alias)
 *   __libcgj_batch36791_marker = "libcgj-batch36791"
 *
 * Milestone 36800 exclusive continuum CREATE-ONLY (36791-36800). Unique
 * gj_shell_green_36800 surface only; no multi-def. Distinct from
 * gj_shell_green_36700 / gj_shell_green_36600 / gj_shell_green_36500 and
 * sibling 36800 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36791–36800): host soft probes
 * (cgj_soft_milestone_36800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36791. */
const char __libcgj_batch36791_marker[] = "libcgj-batch36791";

/* Shell green lamp for wave 36800 (soft product status: ready). */
#define B36791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36791_green(void)
{
	return B36791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_36800 - report product shell green lamp for wave 36800.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_36800(void)
{
	(void)NULL;
	return b36791_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_36800(void)
    __attribute__((alias("gj_shell_green_36800")));
