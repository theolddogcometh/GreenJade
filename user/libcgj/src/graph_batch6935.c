/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6935: splice known flags full catalog mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_splice_f_all_6935(void);
 *     - Returns 0x0000000F (soft MOVE|NONBLOCK|MORE|GIFT catalog OR).
 *       Pure-data catalog stub; not a live splice/tee/vmsplice syscall.
 *   uint32_t __gj_splice_f_all_6935  (alias)
 *   __libcgj_batch6935_marker = "libcgj-batch6935"
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

const char __libcgj_batch6935_marker[] = "libcgj-batch6935";

/*
 * Soft known SPLICE_F_* catalog OR:
 *   MOVE(0x1) | NONBLOCK(0x2) | MORE(0x4) | GIFT(0x8) == 0x0F.
 */
#define B6935_SPLICE_F_ALL  0x0000000fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6935_all(void)
{
	return B6935_SPLICE_F_ALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splice_f_all_6935 - soft full SPLICE_F_* catalog mask.
 *
 * Always returns 0x0000000F. Soft pure-data product tag for the classic
 * four-bit splice/tee/vmsplice flags set. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_splice_f_all_6935(void)
{
	(void)NULL;
	return b6935_all();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_splice_f_all_6935(void)
    __attribute__((alias("gj_splice_f_all_6935")));
