/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8750: batch identity for xattr id-stub wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8750(void);
 *     - Returns the compile-time graph batch number for this TU (8750).
 *   uint32_t __gj_batch_id_8750  (alias)
 *   __libcgj_batch8750_marker = "libcgj-batch8750"
 *
 * Exclusive CREATE-ONLY wave (8741-8750): closes the xattr id stub
 * continuum (create/replace ids, user/sys/sec/trusted/name/size ok
 * lamps, ready). Does NOT redefine gj_batch_id / prior batch_id_* —
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8750_marker[] = "libcgj-batch8750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8750_id(void)
{
	return 8750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8750 — report this TU's graph batch number.
 *
 * Always returns 8750. No parent wires.
 */
uint32_t
gj_batch_id_8750(void)
{
	(void)NULL;
	return b8750_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8750(void)
    __attribute__((alias("gj_batch_id_8750")));
