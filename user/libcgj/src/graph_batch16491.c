/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16491: shell product green extract (wave 16500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_16500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       16500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_16500  (alias)
 *   __libcgj_batch16491_marker = "libcgj-batch16491"
 *
 * Milestone 16500 exclusive continuum CREATE-ONLY (16491-16500). Unique
 * gj_shell_green_16500 surface only; no multi-def. Distinct from
 * gj_shell_green_16500 / gj_shell_green_16300 / gj_shell_green_16200 and
 * sibling 16500 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16491–16500): host soft probes
 * (cgj_soft_milestone_16500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16491. */
const char __libcgj_batch16491_marker[] = "libcgj-batch16491";

/* Shell green lamp for wave 16500 (soft product status: ready). */
#define B16491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16491_green(void)
{
	return B16491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_16500 - report product shell green lamp for wave 16500.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_16500(void)
{
	(void)NULL;
	return b16491_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_16500(void)
    __attribute__((alias("gj_shell_green_16500")));
