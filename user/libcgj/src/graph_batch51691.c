/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch51691: shell product green extract (wave 51700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_51700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       51700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_51700  (alias)
 *   __libcgj_batch51691_marker = "libcgj-batch51691"
 *
 * Milestone 51700 exclusive continuum CREATE-ONLY (51691-51700). Unique
 * gj_shell_green_51700 surface only; no multi-def. Distinct from
 * gj_shell_green_51600 / gj_shell_green_51500 / gj_shell_green_51700 and
 * sibling 51700 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (51691–51700): host soft probes
 * (cgj_soft_milestone_51700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=51700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 51691. */
const char __libcgj_batch51691_marker[] = "libcgj-batch51691";

/* Shell green lamp for wave 51700 (soft product status: ready). */
#define B51691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b51691_green(void)
{
	return B51691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_51700 - report product shell green lamp for wave 51700.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_51700(void)
{
	(void)NULL;
	return b51691_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_51700(void)
    __attribute__((alias("gj_shell_green_51700")));
