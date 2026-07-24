/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27392: libcgj product green extract (wave 27400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_27400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       27400 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_27400  (alias)
 *   __libcgj_batch27392_marker = "libcgj-batch27392"
 *
 * Milestone 27400 exclusive continuum CREATE-ONLY (27391-27400). Unique
 * gj_libcgj_green_27400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_27300 / gj_libcgj_green_27200 / gj_libcgj_green_27100 and
 * sibling 27400 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27391–27400): host soft probes
 * (cgj_soft_milestone_27400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27392. */
const char __libcgj_batch27392_marker[] = "libcgj-batch27392";

/* Libcgj green lamp for wave 27400 (soft product status: ready). */
#define B27392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27392_green(void)
{
	return B27392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_27400 - report product libcgj green lamp for wave 27400.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_27400(void)
{
	(void)NULL;
	return b27392_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_27400(void)
    __attribute__((alias("gj_libcgj_green_27400")));
