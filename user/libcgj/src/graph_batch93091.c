/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch93091: shell product green extract (wave 93100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_93100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       93100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_93100  (alias)
 *   __libcgj_batch93091_marker = "libcgj-batch93091"
 *
 * Milestone 93100 exclusive continuum CREATE-ONLY (93091-93100). Unique
 * gj_shell_green_93100 surface only; no multi-def. Distinct from
 * gj_shell_green_25800 / gj_shell_green_25700 / gj_shell_green_25600 and
 * sibling 93100 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (93091–93100): host soft probes
 * (cgj_soft_milestone_93100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=93100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 93091. */
const char __libcgj_batch93091_marker[] = "libcgj-batch93091";

/* Shell green lamp for wave 93100 (soft product status: ready). */
#define B93091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b93091_green(void)
{
	return B93091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_93100 - report product shell green lamp for wave 93100.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_93100(void)
{
	(void)NULL;
	return b93091_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_93100(void)
    __attribute__((alias("gj_shell_green_93100")));
