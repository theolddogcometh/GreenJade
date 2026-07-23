/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1871: Unix mode regular-file type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_reg_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFREG
 *       (regular file). Compares (mode & S_IFMT) == S_IFREG using the
 *       standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT  = 0170000 (0xF000)  file type mask
 *         S_IFREG = 0100000 (0x8000)  regular file
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_reg_hint  (alias)
 *   __libcgj_batch1871_marker = "libcgj-batch1871"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1871_marker[] = "libcgj-batch1871";

/* Unix st_mode file-type mask / regular-file value (octal). */
#define B1871_IFMT   0170000u
#define B1871_IFREG  0100000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1871_is_reg(uint32_t uMode)
{
	return ((uMode & B1871_IFMT) == B1871_IFREG) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_reg_hint — 1 if mode has S_IFREG file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0100000, else 0.
 */
int
gj_mode_is_reg_hint(uint32_t uMode)
{
	return b1871_is_reg(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_reg_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_reg_hint")));
