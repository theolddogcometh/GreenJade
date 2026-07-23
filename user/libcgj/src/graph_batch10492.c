/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10492: libcgj product green extract (wave 10500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_10500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       10500 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_10500  (alias)
 *   __libcgj_batch10492_marker = "libcgj-batch10492"
 *
 * Milestone 10500 exclusive continuum CREATE-ONLY (10491-10500). Unique
 * gj_libcgj_green_10500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_10400 (batch10392), gj_libcgj_green_10300 (batch10292),
 * gj_libcgj_green_10200 (batch10192), and gj_shell_green_10500
 * (batch10491). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10492_marker[] = "libcgj-batch10492";

/* Product libcgj green lamp for wave 10500. */
#define B10492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10492_green(void)
{
return B10492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_10500 - report product libcgj green lamp for wave 10500.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_10500(void)
{
(void)NULL;
return b10492_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_10500(void)
    __attribute__((alias("gj_libcgj_green_10500")));
