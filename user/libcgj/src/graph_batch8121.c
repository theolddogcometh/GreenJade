/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8121: strncpy pad-byte id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_pad_byte_id_8121(void);
 *     - Return soft strncpy pad fill byte (0 / NUL).
 *   uint32_t __gj_strncpy_pad_byte_id_8121  (alias)
 *   __libcgj_batch8121_marker = "libcgj-batch8121"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_pad_byte_id_8121 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8121_marker[] = "libcgj-batch8121";

/* Soft strncpy remainder pad fill (ISO C zeros remaining bytes). */
#define B8121_STRNCPY_PAD_BYTE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8121_strncpy_pad_byte_id(void)
{
	return B8121_STRNCPY_PAD_BYTE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_pad_byte_id_8121 - soft strncpy pad fill byte.
 *
 * Always returns 0 (NUL). Catalog id only; does not call strncpy(3).
 * No parent wires.
 */
uint32_t
gj_strncpy_pad_byte_id_8121(void)
{
	(void)NULL;
	return b8121_strncpy_pad_byte_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_pad_byte_id_8121(void)
    __attribute__((alias("gj_strncpy_pad_byte_id_8121")));
