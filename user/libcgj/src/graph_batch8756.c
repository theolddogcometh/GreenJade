/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8756: mode S_IXGRP bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_ixgrp_id_8756(void);
 *     - Return soft mode bit id for S_IXGRP (0010 / 0x08).
 *   uint32_t __gj_mode_ixgrp_id_8756  (alias)
 *   __libcgj_batch8756_marker = "libcgj-batch8756"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_ixgrp_id_8756 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8756_marker[] = "libcgj-batch8756";

/* Soft S_IXGRP: group-execute permission (0010). */
#define B8756_S_IXGRP  ((uint32_t)0x00000008u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8756_mode_ixgrp_id(void)
{
	return B8756_S_IXGRP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_ixgrp_id_8756 - soft S_IXGRP mode bit id.
 *
 * Always returns 0x08. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_ixgrp_id_8756(void)
{
	(void)NULL;
	return b8756_mode_ixgrp_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_ixgrp_id_8756(void)
    __attribute__((alias("gj_mode_ixgrp_id_8756")));
