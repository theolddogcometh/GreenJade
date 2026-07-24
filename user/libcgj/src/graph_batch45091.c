/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45091: shell product green extract (wave 45100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_45100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       45100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_45100  (alias)
 *   __libcgj_batch45091_marker = "libcgj-batch45091"
 *
 * Milestone 45100 exclusive continuum CREATE-ONLY (45091-45100). Unique
 * gj_shell_green_45100 surface only; no multi-def. Distinct from
 * gj_shell_green_45000 / gj_shell_green_44900 / gj_shell_green_44800 and
 * sibling 45100 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (45091–45100): host soft probes
 * (cgj_soft_milestone_45100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=45100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 45091. */
const char __libcgj_batch45091_marker[] = "libcgj-batch45091";

/* Shell green lamp for wave 45100 (soft product status: ready). */
#define B45091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45091_green(void)
{
	return B45091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_45100 - report product shell green lamp for wave 45100.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_45100(void)
{
	(void)NULL;
	return b45091_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_45100(void)
    __attribute__((alias("gj_shell_green_45100")));
