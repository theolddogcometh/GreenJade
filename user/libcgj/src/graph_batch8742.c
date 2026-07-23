/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8742: xattr id stub (XATTR_REPLACE flag).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_replace_id_8742(void);
 *     - Returns the traditional XATTR_REPLACE flag as a uint32 tag (2).
 *       Tag only — does not call setxattr/getxattr or any syscall.
 *   uint32_t __gj_xattr_replace_id_8742  (alias)
 *   __libcgj_batch8742_marker = "libcgj-batch8742"
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

const char __libcgj_batch8742_marker[] = "libcgj-batch8742";

/* Traditional XATTR_REPLACE flag value. */
#define B8742_XATTR_REPLACE_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8742_xattr_replace_id(void)
{
	return B8742_XATTR_REPLACE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_replace_id_8742 — report the traditional XATTR_REPLACE flag id.
 *
 * Always returns 2 (XATTR_REPLACE). Tag only; no xattr op is issued.
 * No parent wires.
 */
uint32_t
gj_xattr_replace_id_8742(void)
{
	(void)NULL;
	return b8742_xattr_replace_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_replace_id_8742(void)
    __attribute__((alias("gj_xattr_replace_id_8742")));
