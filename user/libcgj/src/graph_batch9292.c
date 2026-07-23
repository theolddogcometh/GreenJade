/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9292: libcgj product green extract (wave 9300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_9300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       9300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_9300  (alias)
 *   __libcgj_batch9292_marker = "libcgj-batch9292"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_libcgj_green_9300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_9200 (batch9192), gj_libcgj_green_9100 (batch9092),
 * gj_libcgj_green_9000 (batch8992), and gj_shell_green_9300
 * (batch9291). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9292_marker[] = "libcgj-batch9292";

/* Product libcgj green lamp for wave 9300. */
#define B9292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9292_green(void)
{
	return B9292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_9300 - report product libcgj green lamp for wave 9300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_9300(void)
{
	(void)NULL;
	return b9292_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_9300(void)
    __attribute__((alias("gj_libcgj_green_9300")));
