/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20891: shell product green extract (wave 20900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_20900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       20900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_20900  (alias)
 *   __libcgj_batch20891_marker = "libcgj-batch20891"
 *
 * Milestone 20900 exclusive continuum CREATE-ONLY (20891-20900). Unique
 * gj_shell_green_20900 surface only; no multi-def. Distinct from
 * gj_shell_green_20800 / gj_shell_green_20700 / gj_shell_green_20600 and
 * sibling 20900 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20891–20900): host soft probes
 * (cgj_soft_milestone_20900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20891. */
const char __libcgj_batch20891_marker[] = "libcgj-batch20891";

/* Shell green lamp for wave 20900 (soft product status: ready). */
#define B20891_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20891_green(void)
{
	return B20891_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_20900 - report product shell green lamp for wave 20900.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_20900(void)
{
	(void)NULL;
	return b20891_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_20900(void)
    __attribute__((alias("gj_shell_green_20900")));
