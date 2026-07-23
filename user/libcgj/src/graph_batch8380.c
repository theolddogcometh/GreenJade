/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8380: mmap/prot flag stubs wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8380(void);
 *     - Returns the compile-time graph batch number for this TU (8380).
 *   uint32_t __gj_batch_id_8380  (alias)
 *   __libcgj_batch8380_marker = "libcgj-batch8380"
 *
 * Exclusive continuum CREATE-ONLY (8371-8380: mmap/prot flag stubs
 * integer — prot_none/read/write/exec, map_private/shared/anon/fixed,
 * page_align_u, batch_id). Does NOT define gj_batch_id / prior
 * batch_id_* surfaces — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8380_marker[] = "libcgj-batch8380";

#define B8380_BATCH_ID  8380u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8380_id(void)
{
	return B8380_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8380 - report this TU's graph batch number.
 *
 * Always returns 8380. Wave closer for 8371-8380 mmap/prot flag stubs.
 * No parent wires.
 */
uint32_t
gj_batch_id_8380(void)
{
	(void)NULL;
	return b8380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8380(void)
    __attribute__((alias("gj_batch_id_8380")));
