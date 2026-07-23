/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1876: Unix mode character-device type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_chr_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFCHR
 *       (character device). Compares (mode & S_IFMT) == S_IFCHR using
 *       the standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT  = 0170000 (0xF000)  file type mask
 *         S_IFCHR = 0020000 (0x2000)  character special
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_chr_hint  (alias)
 *   __libcgj_batch1876_marker = "libcgj-batch1876"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1876_marker[] = "libcgj-batch1876";

/* Unix st_mode file-type mask / char-device value (octal). */
#define B1876_IFMT   0170000u
#define B1876_IFCHR  0020000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1876_is_chr(uint32_t uMode)
{
	return ((uMode & B1876_IFMT) == B1876_IFCHR) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_chr_hint — 1 if mode has S_IFCHR file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0020000, else 0.
 */
int
gj_mode_is_chr_hint(uint32_t uMode)
{
	return b1876_is_chr(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_chr_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_chr_hint")));
