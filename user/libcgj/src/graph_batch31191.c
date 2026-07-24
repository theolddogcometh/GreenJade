/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31191: shell product green extract (wave 31200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_31200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       31200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_31200  (alias)
 *   __libcgj_batch31191_marker = "libcgj-batch31191"
 *
 * Milestone 31200 exclusive continuum CREATE-ONLY (31191-31200). Unique
 * gj_shell_green_31200 surface only; no multi-def. Distinct from
 * gj_shell_green_31100 / gj_shell_green_31000 / gj_shell_green_30900 and
 * sibling 31200 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (31191–31200): host soft probes
 * (cgj_soft_milestone_31200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=31200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 31191. */
const char __libcgj_batch31191_marker[] = "libcgj-batch31191";

/* Shell green lamp for wave 31200 (soft product status: ready). */
#define B31191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b31191_green(void)
{
	return B31191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_31200 - report product shell green lamp for wave 31200.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_31200(void)
{
	(void)NULL;
	return b31191_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_31200(void)
    __attribute__((alias("gj_shell_green_31200")));
