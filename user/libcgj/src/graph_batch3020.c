/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3020: batch identity for exclusive wave
 * 3011-3020.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3020(void);
 *     - Returns the compile-time graph batch number for this TU (3020).
 *   uint32_t __gj_batch_id_3020  (alias)
 *   __libcgj_batch3020_marker = "libcgj-batch3020"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020 capstone. Distinct
 * from gj_batch_id_3000 and all prior batch_id_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3020_marker[] = "libcgj-batch3020";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3020_id(void)
{
	return 3020u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3020 - report this TU's graph batch number.
 *
 * Always returns 3020.
 */
uint32_t
gj_batch_id_3020(void)
{
	(void)NULL;
	return b3020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3020(void)
    __attribute__((alias("gj_batch_id_3020")));
