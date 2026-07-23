/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8744: xattr system-namespace ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xattr_sys_ok_u_8744(void);
 *     - Returns 1 (system.* xattr namespace soft-ok / accepted tag).
 *       Tag only — does not probe namespaces or call any syscall.
 *   uint32_t __gj_xattr_sys_ok_u_8744  (alias)
 *   __libcgj_batch8744_marker = "libcgj-batch8744"
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

const char __libcgj_batch8744_marker[] = "libcgj-batch8744";

/* Soft ok lamp for the system.* xattr namespace. */
#define B8744_XATTR_SYS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8744_xattr_sys_ok(void)
{
	return B8744_XATTR_SYS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xattr_sys_ok_u_8744 — report system.* xattr namespace soft-ok lamp.
 *
 * Always returns 1 (system namespace accepted). Tag only; no probe.
 * No parent wires.
 */
uint32_t
gj_xattr_sys_ok_u_8744(void)
{
	(void)NULL;
	return b8744_xattr_sys_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xattr_sys_ok_u_8744(void)
    __attribute__((alias("gj_xattr_sys_ok_u_8744")));
