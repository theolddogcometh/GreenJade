/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8030: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8030(void);
 *     - Returns the compile-time graph batch number for this TU (8030).
 *   uint32_t __gj_batch_id_8030  (alias)
 *   __libcgj_batch8030_marker = "libcgj-batch8030"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8030_marker[] = "libcgj-batch8030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8030_id(void)
{
return 8030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8030 - report this TU's graph batch number.
 *
 * Always returns 8030. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8030(void)
{
(void)NULL;
return b8030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8030(void)
    __attribute__((alias("gj_batch_id_8030")));
