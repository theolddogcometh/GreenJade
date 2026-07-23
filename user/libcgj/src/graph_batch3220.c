/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3220: MILESTONE 3220 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3220(void);
 *     - Returns the compile-time graph batch number for this TU (3220).
 *   uint32_t __gj_batch_id_3220  (alias)
 *   __libcgj_batch3220_marker = "libcgj-batch3220"
 *
 * MILESTONE 3220 for the exclusive continuum CREATE-ONLY wave
 * (batches 3211-3219: u64_not_u, u32_and_u, u64_and_u, u32_or_u,
 * u64_or_u, u32_xor_u, u64_xor_u, u32_nand_u, u64_nand_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3220_marker[] = "libcgj-batch3220";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3220_id(void)
{
return 3220u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3220 - report this TU's graph batch number.
 *
 * Always returns 3220. Link-time presence of this symbol tags the wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3220(void)
{
(void)NULL;
return b3220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3220(void)
    __attribute__((alias("gj_batch_id_3220")));
