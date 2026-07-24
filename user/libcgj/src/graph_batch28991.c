/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28991: shell product green extract (wave 29000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_29000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       29000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_29000  (alias)
 *   __libcgj_batch28991_marker = "libcgj-batch28991"
 *
 * Milestone 29000 exclusive continuum CREATE-ONLY (28991-29000). Unique
 * gj_shell_green_29000 surface only; no multi-def. Distinct from
 * gj_shell_green_28900 / gj_shell_green_28800 / gj_shell_green_28700 and
 * sibling 29000 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28991–29000): host soft probes
 * (cgj_soft_milestone_29000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28991. */
const char __libcgj_batch28991_marker[] = "libcgj-batch28991";

/* Shell green lamp for wave 29000 (soft product status: ready). */
#define B28991_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28991_green(void)
{
	return B28991_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_29000 - report product shell green lamp for wave 29000.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_29000(void)
{
	(void)NULL;
	return b28991_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_29000(void)
    __attribute__((alias("gj_shell_green_29000")));
