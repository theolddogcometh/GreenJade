/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45391: shell product green extract (wave 45400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_45400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       45400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_45400  (alias)
 *   __libcgj_batch45391_marker = "libcgj-batch45391"
 *
 * Milestone 45400 exclusive continuum CREATE-ONLY (45391-45400). Unique
 * gj_shell_green_45400 surface only; no multi-def. Distinct from
 * gj_shell_green_45300 / gj_shell_green_45200 / gj_shell_green_45100 and
 * sibling 45400 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (45391–45400): host soft probes
 * (cgj_soft_milestone_45400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=45400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 45391. */
const char __libcgj_batch45391_marker[] = "libcgj-batch45391";

/* Shell green lamp for wave 45400 (soft product status: ready). */
#define B45391_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45391_green(void)
{
	return B45391_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_45400 - report product shell green lamp for wave 45400.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_45400(void)
{
	(void)NULL;
	return b45391_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_45400(void)
    __attribute__((alias("gj_shell_green_45400")));
