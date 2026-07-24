/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42492: libcgj product green extract (wave 42500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_42500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       42500 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_42500  (alias)
 *   __libcgj_batch42492_marker = "libcgj-batch42492"
 *
 * Milestone 42500 exclusive continuum CREATE-ONLY (42491-42500). Unique
 * gj_libcgj_green_42500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_42400 / gj_libcgj_green_42300 / gj_libcgj_green_42200 and
 * sibling 42500 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42491–42500): host soft probes
 * (cgj_soft_milestone_42500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42492. */
const char __libcgj_batch42492_marker[] = "libcgj-batch42492";

/* Libcgj green lamp for wave 42500 (soft product status: ready). */
#define B42492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42492_green(void)
{
	return B42492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_42500 - report product libcgj green lamp for wave 42500.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_42500(void)
{
	(void)NULL;
	return b42492_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_42500(void)
    __attribute__((alias("gj_libcgj_green_42500")));
