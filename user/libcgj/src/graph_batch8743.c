/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8743: xattr user-namespace ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_user_ok_u_8743(void);
 *     - Returns 1 (user.* xattr namespace soft-ok / accepted tag).
 *       Tag only — does not probe namespaces or call any syscall.
 *   uint32_t __gj_xattr_user_ok_u_8743  (alias)
 *   __libcgj_batch8743_marker = "libcgj-batch8743"
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

const char __libcgj_batch8743_marker[] = "libcgj-batch8743";

/* Soft ok lamp for the user.* xattr namespace. */
#define B8743_XATTR_USER_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8743_xattr_user_ok(void)
{
	return B8743_XATTR_USER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_user_ok_u_8743 — report user.* xattr namespace soft-ok lamp.
 *
 * Always returns 1 (user namespace accepted). Tag only; no probe.
 * No parent wires.
 */
uint32_t
gj_xattr_user_ok_u_8743(void)
{
	(void)NULL;
	return b8743_xattr_user_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_user_ok_u_8743(void)
    __attribute__((alias("gj_xattr_user_ok_u_8743")));
