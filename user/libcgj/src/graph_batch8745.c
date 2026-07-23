/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8745: xattr security-namespace ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_sec_ok_u_8745(void);
 *     - Returns 1 (security.* xattr namespace soft-ok / accepted tag).
 *       Tag only — does not probe namespaces or call any syscall.
 *   uint32_t __gj_xattr_sec_ok_u_8745  (alias)
 *   __libcgj_batch8745_marker = "libcgj-batch8745"
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

const char __libcgj_batch8745_marker[] = "libcgj-batch8745";

/* Soft ok lamp for the security.* xattr namespace. */
#define B8745_XATTR_SEC_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8745_xattr_sec_ok(void)
{
	return B8745_XATTR_SEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_sec_ok_u_8745 — report security.* xattr namespace soft-ok lamp.
 *
 * Always returns 1 (security namespace accepted). Tag only; no probe.
 * No parent wires.
 */
uint32_t
gj_xattr_sec_ok_u_8745(void)
{
	(void)NULL;
	return b8745_xattr_sec_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_sec_ok_u_8745(void)
    __attribute__((alias("gj_xattr_sec_ok_u_8745")));
