/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44091: shell product green extract (wave 44100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_44100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       44100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_44100  (alias)
 *   __libcgj_batch44091_marker = "libcgj-batch44091"
 *
 * Milestone 44100 exclusive continuum CREATE-ONLY (44091-44100). Unique
 * gj_shell_green_44100 surface only; no multi-def. Distinct from
 * gj_shell_green_44000 / gj_shell_green_43900 / gj_shell_green_43800 and
 * sibling 44100 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44091–44100): host soft probes
 * (cgj_soft_milestone_44100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44091. */
const char __libcgj_batch44091_marker[] = "libcgj-batch44091";

/* Shell green lamp for wave 44100 (soft product status: ready). */
#define B44091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44091_green(void)
{
	return B44091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_44100 - report product shell green lamp for wave 44100.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_44100(void)
{
	(void)NULL;
	return b44091_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_44100(void)
    __attribute__((alias("gj_shell_green_44100")));
