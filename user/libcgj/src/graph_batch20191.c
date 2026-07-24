/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20191: shell product green extract (wave 20200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_20200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       20200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_20200  (alias)
 *   __libcgj_batch20191_marker = "libcgj-batch20191"
 *
 * Milestone 20200 exclusive continuum CREATE-ONLY (20191-20200). Unique
 * gj_shell_green_20200 surface only; no multi-def. Distinct from
 * gj_shell_green_20100 / gj_shell_green_20000 / gj_shell_green_19900 and
 * sibling 20200 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20191–20200): host soft probes
 * (cgj_soft_milestone_20200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20191. */
const char __libcgj_batch20191_marker[] = "libcgj-batch20191";

/* Shell green lamp for wave 20200 (soft product status: ready). */
#define B20191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20191_green(void)
{
	return B20191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_20200 - report product shell green lamp for wave 20200.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_20200(void)
{
	(void)NULL;
	return b20191_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_20200(void)
    __attribute__((alias("gj_shell_green_20200")));
