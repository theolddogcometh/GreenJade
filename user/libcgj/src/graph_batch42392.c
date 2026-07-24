/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42392: libcgj product green extract (wave 42400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_42400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       42400 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_42400  (alias)
 *   __libcgj_batch42392_marker = "libcgj-batch42392"
 *
 * Milestone 42400 exclusive continuum CREATE-ONLY (42391-42400). Unique
 * gj_libcgj_green_42400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_42300 / gj_libcgj_green_42200 / gj_libcgj_green_42100 and
 * sibling 42400 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42391–42400): host soft probes
 * (cgj_soft_milestone_42400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42392. */
const char __libcgj_batch42392_marker[] = "libcgj-batch42392";

/* Libcgj green lamp for wave 42400 (soft product status: ready). */
#define B42392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42392_green(void)
{
	return B42392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_42400 - report product libcgj green lamp for wave 42400.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_42400(void)
{
	(void)NULL;
	return b42392_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_42400(void)
    __attribute__((alias("gj_libcgj_green_42400")));
