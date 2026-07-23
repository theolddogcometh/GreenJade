/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7321: fcntl F_DUPFD cmd id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_f_dupfd_id_7321(void);
 *     - Return soft F_DUPFD constant (0).
 *   uint32_t __gj_f_dupfd_id_7321  (alias)
 *   __libcgj_batch7321_marker = "libcgj-batch7321"
 *
 * Exclusive continuum CREATE-ONLY (7321-7330: fcntl cmd stubs —
 * dupfd_id, getfd_id, setfd_id, getfl_id, setfl_id, getlk_id,
 * setlk_id, is_getfd, cmd_ok, batch_id_7330).
 * Unique gj_f_dupfd_id_7321 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7321_marker[] = "libcgj-batch7321";

/* F_DUPFD: duplicate file descriptor (value 0 on Linux). */
#define B7321_F_DUPFD ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7321_f_dupfd_id(void)
{
	return B7321_F_DUPFD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_f_dupfd_id_7321 - soft F_DUPFD command constant.
 *
 * Always returns 0 (Linux F_DUPFD). Catalog id only; does not call
 * fcntl. No parent wires.
 */
uint32_t
gj_f_dupfd_id_7321(void)
{
	(void)NULL;
	return b7321_f_dupfd_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_f_dupfd_id_7321(void)
    __attribute__((alias("gj_f_dupfd_id_7321")));
