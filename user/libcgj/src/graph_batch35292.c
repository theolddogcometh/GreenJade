/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35292: libcgj product green extract (wave 35300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_35300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       35300 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_35300  (alias)
 *   __libcgj_batch35292_marker = "libcgj-batch35292"
 *
 * Milestone 35300 exclusive continuum CREATE-ONLY (35291-35300). Unique
 * gj_libcgj_green_35300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_35200 / gj_libcgj_green_35100 / gj_libcgj_green_35000 and
 * sibling 35300 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35291–35300): host soft probes
 * (cgj_soft_milestone_35300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=35300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35292. */
const char __libcgj_batch35292_marker[] = "libcgj-batch35292";

/* Libcgj green lamp for wave 35300 (soft product status: ready). */
#define B35292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35292_green(void)
{
	return B35292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_35300 - report product libcgj green lamp for wave 35300.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_35300(void)
{
	(void)NULL;
	return b35292_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_35300(void)
    __attribute__((alias("gj_libcgj_green_35300")));
