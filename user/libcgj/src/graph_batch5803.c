/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5803: install media ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_media_ready_5803(void);
 *     - Returns 1 (install-media surface ready for the product bar3
 *       install media finalize continuum). Soft compile-time product
 *       tag; not a live block-device or ISO path probe.
 *   uint32_t __gj_install_media_ready_5803  (alias)
 *   __libcgj_batch5803_marker = "libcgj-batch5803"
 *
 * Exclusive continuum CREATE-ONLY (5801-5810: product bar3 install
 * media finalize). Unique gj_install_media_ready_5803 surface only; no
 * multi-def. Distinct from gj_install_media_ready_5404 and
 * gj_install_media_ok (batch2638). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5803_marker[] = "libcgj-batch5803";

/* Install-media surface compiled into this wave. */
#define B5803_MEDIA_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5803_ready(void)
{
	return B5803_MEDIA_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_media_ready_5803 - report install-media ready lamp.
 *
 * Always returns 1 (ready). Integer-only product tag; does not mount
 * media or call libc. No parent wires.
 */
uint32_t
gj_install_media_ready_5803(void)
{
	(void)NULL;
	return b5803_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_media_ready_5803(void)
    __attribute__((alias("gj_install_media_ready_5803")));
