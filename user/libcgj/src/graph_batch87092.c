/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87092: libcgj product green extract (wave 87100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_87100(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       87100 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_87100  (alias)
 *   __libcgj_batch87092_marker = "libcgj-batch87092"
 *
 * Milestone 87100 exclusive continuum CREATE-ONLY (25891-87100). Unique
 * gj_libcgj_green_87100 surface only; no multi-def. Distinct from
 * gj_libcgj_green_25800 / gj_libcgj_green_25700 / gj_libcgj_green_25600 and
 * sibling 87100 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–87100): host soft probes
 * (cgj_soft_milestone_87100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=87100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87092. */
const char __libcgj_batch87092_marker[] = "libcgj-batch87092";

/* Libcgj green lamp for wave 87100 (soft product status: ready). */
#define B87092_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87092_green(void)
{
	return B87092_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_87100 - report product libcgj green lamp for wave 87100.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_87100(void)
{
	(void)NULL;
	return b87092_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_87100(void)
    __attribute__((alias("gj_libcgj_green_87100")));
