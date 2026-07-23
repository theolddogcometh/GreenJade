/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6931: splice SPLICE_F_MOVE flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_splice_f_move_6931(void);
 *     - Returns 0x00000001 (soft SPLICE_F_MOVE flag bit).
 *       Pure-data catalog stub; not a live splice/tee/vmsplice syscall.
 *   uint32_t __gj_splice_f_move_6931  (alias)
 *   __libcgj_batch6931_marker = "libcgj-batch6931"
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

const char __libcgj_batch6931_marker[] = "libcgj-batch6931";

/* Soft SPLICE_F_MOVE (move pages instead of copying). */
#define B6931_SPLICE_F_MOVE  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6931_move(void)
{
	return B6931_SPLICE_F_MOVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splice_f_move_6931 - soft SPLICE_F_MOVE flag catalog bit.
 *
 * Always returns 0x00000001. Pure-data stub for product splice flag
 * catalogs; does not call splice/tee/vmsplice. No parent wires.
 */
uint32_t
gj_splice_f_move_6931(void)
{
	(void)NULL;
	return b6931_move();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_splice_f_move_6931(void)
    __attribute__((alias("gj_splice_f_move_6931")));
