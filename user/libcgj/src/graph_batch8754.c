/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8754: mode S_IRGRP bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_irgrp_id_8754(void);
 *     - Return soft mode bit id for S_IRGRP (0040 / 0x20).
 *   uint32_t __gj_mode_irgrp_id_8754  (alias)
 *   __libcgj_batch8754_marker = "libcgj-batch8754"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_irgrp_id_8754 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8754_marker[] = "libcgj-batch8754";

/* Soft S_IRGRP: group-read permission (0040). */
#define B8754_S_IRGRP  ((uint32_t)0x00000020u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8754_mode_irgrp_id(void)
{
	return B8754_S_IRGRP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_irgrp_id_8754 - soft S_IRGRP mode bit id.
 *
 * Always returns 0x20. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_irgrp_id_8754(void)
{
	(void)NULL;
	return b8754_mode_irgrp_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_irgrp_id_8754(void)
    __attribute__((alias("gj_mode_irgrp_id_8754")));
