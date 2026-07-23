/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7327: fcntl F_SETLK cmd id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_f_setlk_id_7327(void);
 *     - Return soft F_SETLK constant (6).
 *   uint32_t __gj_f_setlk_id_7327  (alias)
 *   __libcgj_batch7327_marker = "libcgj-batch7327"
 *
 * Exclusive continuum CREATE-ONLY (7321-7330: fcntl cmd stubs —
 * dupfd_id, getfd_id, setfd_id, getfl_id, setfl_id, getlk_id,
 * setlk_id, is_getfd, cmd_ok, batch_id_7330).
 * Unique gj_f_setlk_id_7327 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7327_marker[] = "libcgj-batch7327";

/* F_SETLK: set record locking info non-blocking (value 6 on Linux). */
#define B7327_F_SETLK ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7327_f_setlk_id(void)
{
	return B7327_F_SETLK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f_setlk_id_7327 - soft F_SETLK command constant.
 *
 * Always returns 6 (Linux F_SETLK). Catalog id only; does not call
 * fcntl. No parent wires.
 */
uint32_t
gj_f_setlk_id_7327(void)
{
	(void)NULL;
	return b7327_f_setlk_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_f_setlk_id_7327(void)
    __attribute__((alias("gj_f_setlk_id_7327")));
