/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12140: batch identity for IM soft-id wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12140(void);
 *     - Returns the compile-time graph batch number for this TU (12140).
 *   uint32_t __gj_batch_id_12140  (alias)
 *   __libcgj_batch12140_marker = "libcgj-batch12140"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_batch_id_12140 surface only;
 * does not redefine gj_batch_id or prior batch_id_* symbols - avoid
 * multi-def. Distinct from gj_batch_id_11840 (batch11840),
 * gj_batch_id_11640 (batch11640). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12140_marker[] = "libcgj-batch12140";

#define B12140_BATCH_ID  12140u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12140_id(void)
{
	return B12140_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12140 - report this TU's graph batch number.
 *
 * Always returns 12140. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12140(void)
{
	(void)NULL;
	return b12140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12140(void)
    __attribute__((alias("gj_batch_id_12140")));
