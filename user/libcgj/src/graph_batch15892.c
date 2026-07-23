/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15892: libcgj product green extract (wave 15900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_15900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       15900 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_15900  (alias)
 *   __libcgj_batch15892_marker = "libcgj-batch15892"
 *
 * Milestone 15900 exclusive continuum CREATE-ONLY (15891-15900). Unique
 * gj_libcgj_green_15900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_15800 / gj_libcgj_green_15700 / gj_libcgj_green_15600 and
 * sibling 15900 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15891–15900): host soft probes
 * (cgj_soft_milestone_15900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15892. */
const char __libcgj_batch15892_marker[] = "libcgj-batch15892";

/* Libcgj green lamp for wave 15900 (soft product status: ready). */
#define B15892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15892_green(void)
{
	return B15892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_15900 - report product libcgj green lamp for wave 15900.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_15900(void)
{
	(void)NULL;
	return b15892_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_15900(void)
    __attribute__((alias("gj_libcgj_green_15900")));
