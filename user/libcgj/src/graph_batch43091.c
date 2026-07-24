/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43091: shell product green extract (wave 43100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_43100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       43100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_43100  (alias)
 *   __libcgj_batch43091_marker = "libcgj-batch43091"
 *
 * Milestone 43100 exclusive continuum CREATE-ONLY (43091-43100). Unique
 * gj_shell_green_43100 surface only; no multi-def. Distinct from
 * gj_shell_green_43000 / gj_shell_green_42900 / gj_shell_green_42800 and
 * sibling 43100 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (43091–43100): host soft probes
 * (cgj_soft_milestone_43100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=43100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 43091. */
const char __libcgj_batch43091_marker[] = "libcgj-batch43091";

/* Shell green lamp for wave 43100 (soft product status: ready). */
#define B43091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43091_green(void)
{
	return B43091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_43100 - report product shell green lamp for wave 43100.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_43100(void)
{
	(void)NULL;
	return b43091_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_43100(void)
    __attribute__((alias("gj_shell_green_43100")));
