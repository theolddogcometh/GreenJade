/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8746: xattr trusted-namespace ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_trusted_ok_u_8746(void);
 *     - Returns 1 (trusted.* xattr namespace soft-ok / accepted tag).
 *       Tag only — does not probe namespaces or call any syscall.
 *   uint32_t __gj_xattr_trusted_ok_u_8746  (alias)
 *   __libcgj_batch8746_marker = "libcgj-batch8746"
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

const char __libcgj_batch8746_marker[] = "libcgj-batch8746";

/* Soft ok lamp for the trusted.* xattr namespace. */
#define B8746_XATTR_TRUSTED_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8746_xattr_trusted_ok(void)
{
	return B8746_XATTR_TRUSTED_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_trusted_ok_u_8746 — report trusted.* xattr namespace soft-ok.
 *
 * Always returns 1 (trusted namespace accepted). Tag only; no probe.
 * No parent wires.
 */
uint32_t
gj_xattr_trusted_ok_u_8746(void)
{
	(void)NULL;
	return b8746_xattr_trusted_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_trusted_ok_u_8746(void)
    __attribute__((alias("gj_xattr_trusted_ok_u_8746")));
