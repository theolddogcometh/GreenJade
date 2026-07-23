/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8440: crc/poly wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8440(void);
 *     - Returns the compile-time graph batch number for this TU (8440).
 *   uint32_t __gj_batch_id_8440  (alias)
 *   __libcgj_batch8440_marker = "libcgj-batch8440"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs)
 * wave closer. Distinct from gj_batch_id / gj_batch_id_N of other TUs
 * — unique _8440 surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8440_marker[] = "libcgj-batch8440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8440_id(void)
{
	return 8440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8440 - report this TU's graph batch number.
 *
 * Always returns 8440. No parent wires.
 */
uint32_t
gj_batch_id_8440(void)
{
	(void)NULL;
	return b8440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8440(void)
    __attribute__((alias("gj_batch_id_8440")));
