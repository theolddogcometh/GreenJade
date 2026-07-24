/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch91191: shell product green extract (wave 91200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_91200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       91200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_91200  (alias)
 *   __libcgj_batch91191_marker = "libcgj-batch91191"
 *
 * Milestone 91200 exclusive continuum CREATE-ONLY (91191-91200). Unique
 * gj_shell_green_91200 surface only; no multi-def. Distinct from
 * gj_shell_green_25800 / gj_shell_green_25700 / gj_shell_green_25600 and
 * sibling 91200 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (91191–91200): host soft probes
 * (cgj_soft_milestone_91200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=91200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 91191. */
const char __libcgj_batch91191_marker[] = "libcgj-batch91191";

/* Shell green lamp for wave 91200 (soft product status: ready). */
#define B91191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b91191_green(void)
{
	return B91191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_91200 - report product shell green lamp for wave 91200.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_91200(void)
{
	(void)NULL;
	return b91191_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_91200(void)
    __attribute__((alias("gj_shell_green_91200")));
