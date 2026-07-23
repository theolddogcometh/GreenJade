/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7326: fcntl F_GETLK cmd id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_f_getlk_id_7326(void);
 *     - Return soft F_GETLK constant (5).
 *   uint32_t __gj_f_getlk_id_7326  (alias)
 *   __libcgj_batch7326_marker = "libcgj-batch7326"
 *
 * Exclusive continuum CREATE-ONLY (7321-7330: fcntl cmd stubs —
 * dupfd_id, getfd_id, setfd_id, getfl_id, setfl_id, getlk_id,
 * setlk_id, is_getfd, cmd_ok, batch_id_7330).
 * Unique gj_f_getlk_id_7326 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7326_marker[] = "libcgj-batch7326";

/* F_GETLK: get record locking info (value 5 on Linux). */
#define B7326_F_GETLK ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7326_f_getlk_id(void)
{
	return B7326_F_GETLK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f_getlk_id_7326 - soft F_GETLK command constant.
 *
 * Always returns 5 (Linux F_GETLK). Catalog id only; does not call
 * fcntl. No parent wires.
 */
uint32_t
gj_f_getlk_id_7326(void)
{
	(void)NULL;
	return b7326_f_getlk_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_f_getlk_id_7326(void)
    __attribute__((alias("gj_f_getlk_id_7326")));
