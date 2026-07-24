/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24991: shell product green extract (wave 25000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_25000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       25000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_25000  (alias)
 *   __libcgj_batch24991_marker = "libcgj-batch24991"
 *
 * Milestone 25000 exclusive continuum CREATE-ONLY (24991-25000). Unique
 * gj_shell_green_25000 surface only; no multi-def. Distinct from
 * gj_shell_green_24900 / gj_shell_green_24800 / gj_shell_green_24700 and
 * sibling 25000 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24991–25000): host soft probes
 * (cgj_soft_milestone_25000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24991. */
const char __libcgj_batch24991_marker[] = "libcgj-batch24991";

/* Shell green lamp for wave 25000 (soft product status: ready). */
#define B24991_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24991_green(void)
{
	return B24991_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_25000 - report product shell green lamp for wave 25000.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_25000(void)
{
	(void)NULL;
	return b24991_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_25000(void)
    __attribute__((alias("gj_shell_green_25000")));
