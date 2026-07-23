/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6925: copy_file_range fd soft validity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_fd_ok_6925(int32_t fd);
 *     - Return 1 if fd >= 0 (soft non-negative fd check); else 0.
 *   uint32_t __gj_cfr_fd_ok_6925  (alias)
 *   __libcgj_batch6925_marker = "libcgj-batch6925"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_fd_ok_6925 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6925_marker[] = "libcgj-batch6925";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6925_fd_ok(int32_t nFd)
{
	return (nFd >= 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_fd_ok_6925 - soft non-negative fd check for copy_file_range.
 *
 * fd: soft file-descriptor integer (in or out side)
 *
 * Returns 1 when fd is non-negative, else 0. Pure-data gate; does not
 * call fstat or copy_file_range. No parent wires.
 */
uint32_t
gj_cfr_fd_ok_6925(int32_t nFd)
{
	(void)NULL;
	return b6925_fd_ok(nFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_fd_ok_6925(int32_t nFd)
    __attribute__((alias("gj_cfr_fd_ok_6925")));
