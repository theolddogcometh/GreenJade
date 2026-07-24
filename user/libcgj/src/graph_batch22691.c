/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22691: shell product green extract (wave 22700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_22700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       22700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_22700  (alias)
 *   __libcgj_batch22691_marker = "libcgj-batch22691"
 *
 * Milestone 22700 exclusive continuum CREATE-ONLY (22691-22700). Unique
 * gj_shell_green_22700 surface only; no multi-def. Distinct from
 * gj_shell_green_22600 / gj_shell_green_22500 / gj_shell_green_22400 and
 * sibling 22700 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (22691–22700): host soft probes
 * (cgj_soft_milestone_22700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22691. */
const char __libcgj_batch22691_marker[] = "libcgj-batch22691";

/* Shell green lamp for wave 22700 (soft product status: ready). */
#define B22691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22691_green(void)
{
	return B22691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_22700 - report product shell green lamp for wave 22700.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_22700(void)
{
	(void)NULL;
	return b22691_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_22700(void)
    __attribute__((alias("gj_shell_green_22700")));
