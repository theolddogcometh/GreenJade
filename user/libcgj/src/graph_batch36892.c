/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36892: libcgj product green extract (wave 36900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_36900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       36900 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_36900  (alias)
 *   __libcgj_batch36892_marker = "libcgj-batch36892"
 *
 * Milestone 36900 exclusive continuum CREATE-ONLY (36891-36900). Unique
 * gj_libcgj_green_36900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_36800 / gj_libcgj_green_36700 / gj_libcgj_green_36600 and
 * sibling 36900 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36891–36900): host soft probes
 * (cgj_soft_milestone_36900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36892. */
const char __libcgj_batch36892_marker[] = "libcgj-batch36892";

/* Libcgj green lamp for wave 36900 (soft product status: ready). */
#define B36892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36892_green(void)
{
	return B36892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_36900 - report product libcgj green lamp for wave 36900.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_36900(void)
{
	(void)NULL;
	return b36892_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_36900(void)
    __attribute__((alias("gj_libcgj_green_36900")));
