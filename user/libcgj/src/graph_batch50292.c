/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50292: libcgj product green extract (wave 50300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_50300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       50300 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_50300  (alias)
 *   __libcgj_batch50292_marker = "libcgj-batch50292"
 *
 * Milestone 50300 exclusive continuum CREATE-ONLY (50291-50300). Unique
 * gj_libcgj_green_50300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_50200 / gj_libcgj_green_50100 / gj_libcgj_green_50300 and
 * sibling 50300 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50291–50300): host soft probes
 * (cgj_soft_milestone_50300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50292. */
const char __libcgj_batch50292_marker[] = "libcgj-batch50292";

/* Libcgj green lamp for wave 50300 (soft product status: ready). */
#define B50292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50292_green(void)
{
	return B50292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_50300 - report product libcgj green lamp for wave 50300.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_50300(void)
{
	(void)NULL;
	return b50292_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_50300(void)
    __attribute__((alias("gj_libcgj_green_50300")));
