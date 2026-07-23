/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1875: Unix mode FIFO/pipe type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_fifo_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFIFO
 *       (FIFO / named pipe). Compares (mode & S_IFMT) == S_IFIFO using
 *       the standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT  = 0170000 (0xF000)  file type mask
 *         S_IFIFO = 0010000 (0x1000)  FIFO / pipe
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_fifo_hint  (alias)
 *   __libcgj_batch1875_marker = "libcgj-batch1875"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1875_marker[] = "libcgj-batch1875";

/* Unix st_mode file-type mask / FIFO value (octal). */
#define B1875_IFMT   0170000u
#define B1875_IFIFO  0010000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1875_is_fifo(uint32_t uMode)
{
	return ((uMode & B1875_IFMT) == B1875_IFIFO) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_fifo_hint — 1 if mode has S_IFIFO file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0010000, else 0.
 */
int
gj_mode_is_fifo_hint(uint32_t uMode)
{
	return b1875_is_fifo(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_fifo_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_fifo_hint")));
