/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8758: mode S_IWOTH bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_iwoth_id_8758(void);
 *     - Return soft mode bit id for S_IWOTH (0002 / 0x02).
 *   uint32_t __gj_mode_iwoth_id_8758  (alias)
 *   __libcgj_batch8758_marker = "libcgj-batch8758"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_iwoth_id_8758 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8758_marker[] = "libcgj-batch8758";

/* Soft S_IWOTH: other-write permission (0002). */
#define B8758_S_IWOTH  ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8758_mode_iwoth_id(void)
{
	return B8758_S_IWOTH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_iwoth_id_8758 - soft S_IWOTH mode bit id.
 *
 * Always returns 0x02. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_iwoth_id_8758(void)
{
	(void)NULL;
	return b8758_mode_iwoth_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_iwoth_id_8758(void)
    __attribute__((alias("gj_mode_iwoth_id_8758")));
