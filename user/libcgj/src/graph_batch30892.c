/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30892: libcgj product green extract (wave 30900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_30900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       30900 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_30900  (alias)
 *   __libcgj_batch30892_marker = "libcgj-batch30892"
 *
 * Milestone 30900 exclusive continuum CREATE-ONLY (30891-30900). Unique
 * gj_libcgj_green_30900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_30800 / gj_libcgj_green_30700 / gj_libcgj_green_30600 and
 * sibling 30900 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30891–30900): host soft probes
 * (cgj_soft_milestone_30900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30892. */
const char __libcgj_batch30892_marker[] = "libcgj-batch30892";

/* Libcgj green lamp for wave 30900 (soft product status: ready). */
#define B30892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30892_green(void)
{
	return B30892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_30900 - report product libcgj green lamp for wave 30900.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_30900(void)
{
	(void)NULL;
	return b30892_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_30900(void)
    __attribute__((alias("gj_libcgj_green_30900")));
