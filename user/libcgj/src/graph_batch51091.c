/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch51091: shell product green extract (wave 51100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_51100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       51100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_51100  (alias)
 *   __libcgj_batch51091_marker = "libcgj-batch51091"
 *
 * Milestone 51100 exclusive continuum CREATE-ONLY (51091-51100). Unique
 * gj_shell_green_51100 surface only; no multi-def. Distinct from
 * gj_shell_green_51000 / gj_shell_green_50900 / gj_shell_green_50800 and
 * sibling 51100 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (51091–51100): host soft probes
 * (cgj_soft_milestone_51100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=51100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 51091. */
const char __libcgj_batch51091_marker[] = "libcgj-batch51091";

/* Shell green lamp for wave 51100 (soft product status: ready). */
#define B51091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b51091_green(void)
{
	return B51091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_51100 - report product shell green lamp for wave 51100.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_51100(void)
{
	(void)NULL;
	return b51091_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_51100(void)
    __attribute__((alias("gj_shell_green_51100")));
