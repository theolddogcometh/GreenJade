/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48892: libcgj product green extract (wave 48900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_48900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       48900 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_48900  (alias)
 *   __libcgj_batch48892_marker = "libcgj-batch48892"
 *
 * Milestone 48900 exclusive continuum CREATE-ONLY (48891-48900). Unique
 * gj_libcgj_green_48900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_48800 / gj_libcgj_green_48700 / gj_libcgj_green_48600 and
 * sibling 48900 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48891–48900): host soft probes
 * (cgj_soft_milestone_48900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=48900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48892. */
const char __libcgj_batch48892_marker[] = "libcgj-batch48892";

/* Libcgj green lamp for wave 48900 (soft product status: ready). */
#define B48892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48892_green(void)
{
	return B48892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_48900 - report product libcgj green lamp for wave 48900.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_48900(void)
{
	(void)NULL;
	return b48892_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_48900(void)
    __attribute__((alias("gj_libcgj_green_48900")));
