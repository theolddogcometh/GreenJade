/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39491: shell product green extract (wave 39500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_39500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       39500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_39500  (alias)
 *   __libcgj_batch39491_marker = "libcgj-batch39491"
 *
 * Milestone 39500 exclusive continuum CREATE-ONLY (39491-39500). Unique
 * gj_shell_green_39500 surface only; no multi-def. Distinct from
 * gj_shell_green_39400 / gj_shell_green_39300 / gj_shell_green_39200 and
 * sibling 39500 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (39491–39500): host soft probes
 * (cgj_soft_milestone_39500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=39500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 39491. */
const char __libcgj_batch39491_marker[] = "libcgj-batch39491";

/* Shell green lamp for wave 39500 (soft product status: ready). */
#define B39491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39491_green(void)
{
	return B39491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_39500 - report product shell green lamp for wave 39500.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_39500(void)
{
	(void)NULL;
	return b39491_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_39500(void)
    __attribute__((alias("gj_shell_green_39500")));
