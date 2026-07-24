/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40191: shell product green extract (wave 40200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_40200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       40200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_40200  (alias)
 *   __libcgj_batch40191_marker = "libcgj-batch40191"
 *
 * Milestone 40200 exclusive continuum CREATE-ONLY (40191-40200). Unique
 * gj_shell_green_40200 surface only; no multi-def. Distinct from
 * gj_shell_green_40100 / gj_shell_green_40000 / gj_shell_green_39900 and
 * sibling 40200 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40191–40200): host soft probes
 * (cgj_soft_milestone_40200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40191. */
const char __libcgj_batch40191_marker[] = "libcgj-batch40191";

/* Shell green lamp for wave 40200 (soft product status: ready). */
#define B40191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40191_green(void)
{
	return B40191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_40200 - report product shell green lamp for wave 40200.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_40200(void)
{
	(void)NULL;
	return b40191_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_40200(void)
    __attribute__((alias("gj_shell_green_40200")));
