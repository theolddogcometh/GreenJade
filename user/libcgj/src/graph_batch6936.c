/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6936: splice flag catalog membership check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_splice_f_ok_6936(uint32_t flags);
 *     - Returns 1 when flags has only known soft SPLICE_F_* bits
 *       (MOVE|NONBLOCK|MORE|GIFT == 0x0F), else 0.
 *       Pure-data mask stub; not a live splice validation.
 *   uint32_t __gj_splice_f_ok_6936  (alias)
 *   __libcgj_batch6936_marker = "libcgj-batch6936"
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

const char __libcgj_batch6936_marker[] = "libcgj-batch6936";

/*
 * Soft known SPLICE_F_* bits from this exclusive wave catalog:
 * MOVE|NONBLOCK|MORE|GIFT == 0x0F.
 */
#define B6936_SPLICE_F_KNOWN  0x0000000fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6936_flag_ok(uint32_t uFlags)
{
	return ((uFlags & ~B6936_SPLICE_F_KNOWN) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splice_f_ok_6936 - soft SPLICE_F flag catalog membership.
 *
 * flags: candidate splice/tee/vmsplice flags word.
 * Returns 1 if every set bit is in the known soft catalog mask
 * 0x0000000F, else 0. Does not query the kernel. No parent wires.
 */
uint32_t
gj_splice_f_ok_6936(uint32_t flags)
{
	(void)NULL;
	return b6936_flag_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_splice_f_ok_6936(uint32_t flags)
    __attribute__((alias("gj_splice_f_ok_6936")));
