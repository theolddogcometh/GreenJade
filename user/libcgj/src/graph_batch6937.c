/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6937: splice flags has-all bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_splice_f_has_6937(uint32_t flags, uint32_t need);
 *     - Return 1 if (flags & need) == need (all required SPLICE_F bits
 *       present). Soft pure-data has-all; not a live splice check.
 *   uint32_t __gj_splice_f_has_6937  (alias)
 *   __libcgj_batch6937_marker = "libcgj-batch6937"
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

const char __libcgj_batch6937_marker[] = "libcgj-batch6937";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6937_has(uint32_t uFlags, uint32_t uNeed)
{
	return ((uFlags & uNeed) == uNeed) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splice_f_has_6937 - true if flags covers all bits in need.
 *
 * flags: soft splice/tee/vmsplice flags word
 * need:  required SPLICE_F_* bits
 *
 * Returns 1 when every bit of need is set in flags; else 0.
 * Soft pure-data has-all; no syscall. No parent wires.
 */
uint32_t
gj_splice_f_has_6937(uint32_t flags, uint32_t need)
{
	(void)NULL;
	return b6937_has(flags, need);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_splice_f_has_6937(uint32_t flags, uint32_t need)
    __attribute__((alias("gj_splice_f_has_6937")));
