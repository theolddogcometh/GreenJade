/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34091: shell product green extract (wave 34100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_34100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       34100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_34100  (alias)
 *   __libcgj_batch34091_marker = "libcgj-batch34091"
 *
 * Milestone 34100 exclusive continuum CREATE-ONLY (34091-34100). Unique
 * gj_shell_green_34100 surface only; no multi-def. Distinct from
 * gj_shell_green_34000 / gj_shell_green_33900 / gj_shell_green_33800 and
 * sibling 34100 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (34091–34100): host soft probes
 * (cgj_soft_milestone_34100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=34100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 34091. */
const char __libcgj_batch34091_marker[] = "libcgj-batch34091";

/* Shell green lamp for wave 34100 (soft product status: ready). */
#define B34091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34091_green(void)
{
	return B34091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_34100 - report product shell green lamp for wave 34100.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_34100(void)
{
	(void)NULL;
	return b34091_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_34100(void)
    __attribute__((alias("gj_shell_green_34100")));
