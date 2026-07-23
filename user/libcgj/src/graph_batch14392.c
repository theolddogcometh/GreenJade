/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14392: libcgj product green extract (wave 14400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_14400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       14400 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_14400  (alias)
 *   __libcgj_batch14392_marker = "libcgj-batch14392"
 *
 * Milestone 14400 exclusive continuum CREATE-ONLY (14391-14400). Unique
 * gj_libcgj_green_14400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_14000 (batch13992), gj_libcgj_green_13000 (batch12992),
 * gj_libcgj_green_12900 (batch12892), and gj_shell_green_14400
 * (batch14391). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14392_marker[] = "libcgj-batch14392";

/* Product libcgj green lamp for wave 14400. */
#define B14392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14392_green(void)
{
	return B14392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_14400 - report product libcgj green lamp for wave 14400.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_14400(void)
{
	(void)NULL;
	return b14392_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_14400(void)
    __attribute__((alias("gj_libcgj_green_14400")));
