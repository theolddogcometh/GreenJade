/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1874: Unix mode socket type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_sock_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFSOCK
 *       (socket). Compares (mode & S_IFMT) == S_IFSOCK using the
 *       standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT   = 0170000 (0xF000)  file type mask
 *         S_IFSOCK = 0140000 (0xC000)  socket
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_sock_hint  (alias)
 *   __libcgj_batch1874_marker = "libcgj-batch1874"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1874_marker[] = "libcgj-batch1874";

/* Unix st_mode file-type mask / socket value (octal). */
#define B1874_IFMT    0170000u
#define B1874_IFSOCK  0140000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1874_is_sock(uint32_t uMode)
{
	return ((uMode & B1874_IFMT) == B1874_IFSOCK) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_sock_hint — 1 if mode has S_IFSOCK file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0140000, else 0.
 */
int
gj_mode_is_sock_hint(uint32_t uMode)
{
	return b1874_is_sock(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_sock_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_sock_hint")));
