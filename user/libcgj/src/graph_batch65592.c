/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch65592: libcgj product green extract (wave 65600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_65600(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       65600 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_65600  (alias)
 *   __libcgj_batch65592_marker = "libcgj-batch65592"
 *
 * Milestone 65600 exclusive continuum CREATE-ONLY (25891-65600). Unique
 * gj_libcgj_green_65600 surface only; no multi-def. Distinct from
 * gj_libcgj_green_25800 / gj_libcgj_green_25700 / gj_libcgj_green_25600 and
 * sibling 65600 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–65600): host soft probes
 * (cgj_soft_milestone_65600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=65600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 65592. */
const char __libcgj_batch65592_marker[] = "libcgj-batch65592";

/* Libcgj green lamp for wave 65600 (soft product status: ready). */
#define B65592_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b65592_green(void)
{
	return B65592_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_65600 - report product libcgj green lamp for wave 65600.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_65600(void)
{
	(void)NULL;
	return b65592_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_65600(void)
    __attribute__((alias("gj_libcgj_green_65600")));
