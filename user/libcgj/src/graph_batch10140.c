/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10140: batch identity for IM soft-id wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10140(void);
 *     - Returns the compile-time graph batch number for this TU (10140).
 *   uint32_t __gj_batch_id_10140  (alias)
 *   __libcgj_batch10140_marker = "libcgj-batch10140"
 *
 * Exclusive CREATE-ONLY wave 10131-10140 (ibus/fcitx soft id stubs).
 * Unique gj_batch_id_10140 surface only; does not redefine gj_batch_id
 * or prior batch_id_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10140_marker[] = "libcgj-batch10140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10140_id(void)
{
	return 10140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10140 - report this TU's graph batch number.
 *
 * Always returns 10140. No parent wires.
 */
uint32_t
gj_batch_id_10140(void)
{
	(void)NULL;
	return b10140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10140(void)
    __attribute__((alias("gj_batch_id_10140")));
