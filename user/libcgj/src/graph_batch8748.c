/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8748: xattr size soft-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_size_ok_u_8748(void);
 *     - Returns 1 (xattr size soft-ok / accepted tag). Tag only —
 *       does not measure any value buffer or call any syscall.
 *   uint32_t __gj_xattr_size_ok_u_8748  (alias)
 *   __libcgj_batch8748_marker = "libcgj-batch8748"
 *
 * Exclusive CREATE-ONLY wave (8741-8750): xattr id stubs (create/
 * replace flags, user/sys/sec/trusted/name/size ok lamps, ready, batch
 * id). Does NOT redefine prior gj_xattr_* / batch_id_* symbols —
 * avoid multi-def. No parent wires. No xattr syscalls.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8748_marker[] = "libcgj-batch8748";

/* Soft ok lamp for xattr size acceptance. */
#define B8748_XATTR_SIZE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8748_xattr_size_ok(void)
{
	return B8748_XATTR_SIZE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_size_ok_u_8748 — report xattr size soft-ok lamp.
 *
 * Always returns 1 (size soft-ok). Tag only; no buffer measurement.
 * No parent wires.
 */
uint32_t
gj_xattr_size_ok_u_8748(void)
{
	(void)NULL;
	return b8748_xattr_size_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_size_ok_u_8748(void)
    __attribute__((alias("gj_xattr_size_ok_u_8748")));
