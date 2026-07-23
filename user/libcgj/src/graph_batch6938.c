/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6938: tee flags catalog membership check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tee_f_ok_6938(uint32_t flags);
 *     - Returns 1 when flags has only known soft SPLICE_F_* bits shared
 *       by tee(2) (MOVE|NONBLOCK|MORE|GIFT == 0x0F), else 0.
 *       Pure-data mask stub; not a live tee syscall.
 *   uint32_t __gj_tee_f_ok_6938  (alias)
 *   __libcgj_batch6938_marker = "libcgj-batch6938"
 *
 * Exclusive continuum CREATE-ONLY (6931-6940: splice/tee/vmsplice flags
 * stubs —
 * splice_f_move_6931, splice_f_nonblock_6932, splice_f_more_6933,
 * splice_f_gift_6934, splice_f_all_6935, splice_f_ok_6936,
 * splice_f_has_6937, tee_f_ok_6938, vmsplice_f_ok_6939,
 * continuum + batch_id_6940). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6938_marker[] = "libcgj-batch6938";

/*
 * Soft known tee(2) flags share the classic SPLICE_F_* catalog:
 * MOVE|NONBLOCK|MORE|GIFT == 0x0F.
 */
#define B6938_TEE_F_KNOWN  0x0000000fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6938_tee_ok(uint32_t uFlags)
{
	return ((uFlags & ~B6938_TEE_F_KNOWN) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tee_f_ok_6938 - soft tee(2) flags catalog membership.
 *
 * flags: candidate tee flags word (SPLICE_F_* shared catalog).
 * Returns 1 if every set bit is in the known soft catalog mask
 * 0x0000000F, else 0. Does not call tee. No parent wires.
 */
uint32_t
gj_tee_f_ok_6938(uint32_t flags)
{
	(void)NULL;
	return b6938_tee_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tee_f_ok_6938(uint32_t flags)
    __attribute__((alias("gj_tee_f_ok_6938")));
