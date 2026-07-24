/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22892: libcgj product green extract (wave 22900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_22900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       22900 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_22900  (alias)
 *   __libcgj_batch22892_marker = "libcgj-batch22892"
 *
 * Milestone 22900 exclusive continuum CREATE-ONLY (22891-22900). Unique
 * gj_libcgj_green_22900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_22800 / gj_libcgj_green_22700 / gj_libcgj_green_22600 and
 * sibling 22900 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (22891–22900): host soft probes
 * (cgj_soft_milestone_22900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22892. */
const char __libcgj_batch22892_marker[] = "libcgj-batch22892";

/* Libcgj green lamp for wave 22900 (soft product status: ready). */
#define B22892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22892_green(void)
{
	return B22892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_22900 - report product libcgj green lamp for wave 22900.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_22900(void)
{
	(void)NULL;
	return b22892_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_22900(void)
    __attribute__((alias("gj_libcgj_green_22900")));
