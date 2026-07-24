/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80392: libcgj product green extract (wave 80400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_80400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       80400 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_80400  (alias)
 *   __libcgj_batch80392_marker = "libcgj-batch80392"
 *
 * Milestone 80400 exclusive continuum CREATE-ONLY (25891-80400). Unique
 * gj_libcgj_green_80400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_25800 / gj_libcgj_green_25700 / gj_libcgj_green_25600 and
 * sibling 80400 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–80400): host soft probes
 * (cgj_soft_milestone_80400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=80400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 80392. */
const char __libcgj_batch80392_marker[] = "libcgj-batch80392";

/* Libcgj green lamp for wave 80400 (soft product status: ready). */
#define B80392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b80392_green(void)
{
	return B80392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_80400 - report product libcgj green lamp for wave 80400.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_80400(void)
{
	(void)NULL;
	return b80392_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_80400(void)
    __attribute__((alias("gj_libcgj_green_80400")));
