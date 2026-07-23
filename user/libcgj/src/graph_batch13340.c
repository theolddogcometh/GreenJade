/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13340: batch identity for IM soft-id wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13340(void);
 *     - Returns the compile-time graph batch number for this TU (13340).
 *   uint32_t __gj_batch_id_13340  (alias)
 *   __libcgj_batch13340_marker = "libcgj-batch13340"
 *
 * Exclusive CREATE-ONLY wave 13331-13340 (ibus/fcitx soft id stubs).
 * Unique gj_batch_id_13340 surface only; does not redefine gj_batch_id
 * or prior batch_id_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13340_marker[] = "libcgj-batch13340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13340_id(void)
{
	return 13340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13340 - report this TU's graph batch number.
 *
 * Always returns 13340. No parent wires.
 */
uint32_t
gj_batch_id_13340(void)
{
	(void)NULL;
	return b13340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13340(void)
    __attribute__((alias("gj_batch_id_13340")));
