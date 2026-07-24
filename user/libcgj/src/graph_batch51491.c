/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch51491: shell product green extract (wave 51500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_51500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       51500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_51500  (alias)
 *   __libcgj_batch51491_marker = "libcgj-batch51491"
 *
 * Milestone 51500 exclusive continuum CREATE-ONLY (51491-51500). Unique
 * gj_shell_green_51500 surface only; no multi-def. Distinct from
 * gj_shell_green_51400 / gj_shell_green_51300 / gj_shell_green_51200 and
 * sibling 51500 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (51491–51500): host soft probes
 * (cgj_soft_milestone_51500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=51500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 51491. */
const char __libcgj_batch51491_marker[] = "libcgj-batch51491";

/* Shell green lamp for wave 51500 (soft product status: ready). */
#define B51491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b51491_green(void)
{
	return B51491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_51500 - report product shell green lamp for wave 51500.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_51500(void)
{
	(void)NULL;
	return b51491_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_51500(void)
    __attribute__((alias("gj_shell_green_51500")));
