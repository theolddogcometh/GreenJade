/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8759: mode S_IXOTH bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_ixoth_id_8759(void);
 *     - Return soft mode bit id for S_IXOTH (0001 / 0x01).
 *   uint32_t __gj_mode_ixoth_id_8759  (alias)
 *   __libcgj_batch8759_marker = "libcgj-batch8759"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_ixoth_id_8759 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8759_marker[] = "libcgj-batch8759";

/* Soft S_IXOTH: other-execute permission (0001). */
#define B8759_S_IXOTH  ((uint32_t)0x00000001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8759_mode_ixoth_id(void)
{
	return B8759_S_IXOTH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_ixoth_id_8759 - soft S_IXOTH mode bit id.
 *
 * Always returns 0x01. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_ixoth_id_8759(void)
{
	(void)NULL;
	return b8759_mode_ixoth_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_ixoth_id_8759(void)
    __attribute__((alias("gj_mode_ixoth_id_8759")));
