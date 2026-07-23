/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8741: xattr id stub (XATTR_CREATE flag).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_create_id_8741(void);
 *     - Returns the traditional XATTR_CREATE flag as a uint32 tag (1).
 *       Tag only — does not call setxattr/getxattr or any syscall.
 *   uint32_t __gj_xattr_create_id_8741  (alias)
 *   __libcgj_batch8741_marker = "libcgj-batch8741"
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

const char __libcgj_batch8741_marker[] = "libcgj-batch8741";

/* Traditional XATTR_CREATE flag value. */
#define B8741_XATTR_CREATE_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8741_xattr_create_id(void)
{
	return B8741_XATTR_CREATE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_create_id_8741 — report the traditional XATTR_CREATE flag id.
 *
 * Always returns 1 (XATTR_CREATE). Tag only; no xattr op is issued.
 * No parent wires.
 */
uint32_t
gj_xattr_create_id_8741(void)
{
	(void)NULL;
	return b8741_xattr_create_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_create_id_8741(void)
    __attribute__((alias("gj_xattr_create_id_8741")));
