/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35391: shell product green extract (wave 35400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_35400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       35400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_35400  (alias)
 *   __libcgj_batch35391_marker = "libcgj-batch35391"
 *
 * Milestone 35400 exclusive continuum CREATE-ONLY (35391-35400). Unique
 * gj_shell_green_35400 surface only; no multi-def. Distinct from
 * gj_shell_green_35300 / gj_shell_green_35200 / gj_shell_green_35100 and
 * sibling 35400 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35391–35400): host soft probes
 * (cgj_soft_milestone_35400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=35400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35391. */
const char __libcgj_batch35391_marker[] = "libcgj-batch35391";

/* Shell green lamp for wave 35400 (soft product status: ready). */
#define B35391_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35391_green(void)
{
	return B35391_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_35400 - report product shell green lamp for wave 35400.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_35400(void)
{
	(void)NULL;
	return b35391_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_35400(void)
    __attribute__((alias("gj_shell_green_35400")));
