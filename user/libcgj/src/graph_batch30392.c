/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30392: libcgj product green extract (wave 30400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_30400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       30400 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_30400  (alias)
 *   __libcgj_batch30392_marker = "libcgj-batch30392"
 *
 * Milestone 30400 exclusive continuum CREATE-ONLY (30391-30400). Unique
 * gj_libcgj_green_30400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_30300 / gj_libcgj_green_30200 / gj_libcgj_green_30100 and
 * sibling 30400 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30391–30400): host soft probes
 * (cgj_soft_milestone_30400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30392. */
const char __libcgj_batch30392_marker[] = "libcgj-batch30392";

/* Libcgj green lamp for wave 30400 (soft product status: ready). */
#define B30392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30392_green(void)
{
	return B30392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_30400 - report product libcgj green lamp for wave 30400.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_30400(void)
{
	(void)NULL;
	return b30392_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_30400(void)
    __attribute__((alias("gj_libcgj_green_30400")));
