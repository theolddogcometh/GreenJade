/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8650: batch identity for syscall-number wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8650(void);
 *     - Returns the compile-time graph batch number for this TU (8650).
 *   uint32_t __gj_batch_id_8650  (alias)
 *   __libcgj_batch8650_marker = "libcgj-batch8650"
 *
 * Exclusive CREATE-ONLY wave (8641-8650): closes the x86_64 syscall
 * number id stub continuum (read/write/open/close/stat/fstat/mmap/
 * mprotect/exit). Does NOT redefine gj_batch_id / prior batch_id_* —
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8650_marker[] = "libcgj-batch8650";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8650_id(void)
{
	return 8650u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8650 — report this TU's graph batch number.
 *
 * Always returns 8650. No parent wires.
 */
uint32_t
gj_batch_id_8650(void)
{
	(void)NULL;
	return b8650_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8650(void)
    __attribute__((alias("gj_batch_id_8650")));
