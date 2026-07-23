/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3210: batch identity for wrap-arith wave end.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3210(void);
 *     - Returns the compile-time graph batch number for this TU (3210).
 *   uint32_t __gj_batch_id_3210  (alias)
 *   __libcgj_batch3210_marker = "libcgj-batch3210"
 *
 * Exclusive continuum CREATE-ONLY wave (3201-3210: u32/u64 add/sub/mul
 * wrap_u, u32/u64 neg_wrap_u, u32_not_u, batch_id_3210). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols -
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3210_marker[] = "libcgj-batch3210";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3210_id(void)
{
return 3210u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3210 - report this TU's graph batch number.
 *
 * Always returns 3210. End-of-wave identity for wrap-arith exclusive
 * continuum 3201-3210. No parent wires.
 */
uint32_t
gj_batch_id_3210(void)
{
(void)NULL;
return b3210_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3210(void)
    __attribute__((alias("gj_batch_id_3210")));
