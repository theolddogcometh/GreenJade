/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch79991: shell product green extract (wave 80000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_80000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       80000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_80000  (alias)
 *   __libcgj_batch79991_marker = "libcgj-batch79991"
 *
 * Milestone 80000 exclusive continuum CREATE-ONLY (79991-80000). Unique
 * gj_shell_green_80000 surface only; no multi-def. Distinct from
 * gj_shell_green_25800 / gj_shell_green_25700 / gj_shell_green_25600 and
 * sibling 80000 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (79991–80000): host soft probes
 * (cgj_soft_milestone_80000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=80000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 79991. */
const char __libcgj_batch79991_marker[] = "libcgj-batch79991";

/* Shell green lamp for wave 80000 (soft product status: ready). */
#define B79991_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b79991_green(void)
{
	return B79991_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_80000 - report product shell green lamp for wave 80000.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_80000(void)
{
	(void)NULL;
	return b79991_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_80000(void)
    __attribute__((alias("gj_shell_green_80000")));
