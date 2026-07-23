/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13940: batch identity for IM soft-id wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13940(void);
 *     - Returns the compile-time graph batch number for this TU (13940).
 *   uint32_t __gj_batch_id_13940  (alias)
 *   __libcgj_batch13940_marker = "libcgj-batch13940"
 *
 * Exclusive CREATE-ONLY wave 13931-13940 (ibus/fcitx soft id stubs).
 * Unique gj_batch_id_13940 surface only; does not redefine gj_batch_id
 * or prior batch_id_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13940_marker[] = "libcgj-batch13940";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13940_id(void)
{
	return 13940u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13940 - report this TU's graph batch number.
 *
 * Always returns 13940. No parent wires.
 */
uint32_t
gj_batch_id_13940(void)
{
	(void)NULL;
	return b13940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13940(void)
    __attribute__((alias("gj_batch_id_13940")));
