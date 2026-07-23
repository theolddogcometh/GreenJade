/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2638: install media path soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_install_media_ok(const char *path);
 *     - Return 1 if path is a non-NULL absolute path of length >= 2
 *       (leading '/' plus at least one more character). Soft product
 *       install-media path shape (USB image, ISO, ESP payload root);
 *       does not open or probe devices.
 *   int __gj_install_media_ok  (alias)
 *   __libcgj_batch2638_marker = "libcgj-batch2638"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_install_stage_ok (batch1994), gj_install_ready_bits (batch2598),
 * gj_install_stage_advance / is_done (batch2361/2362), and
 * gj_rootfs_path_ok (batch2363, allows "/") — unique
 * gj_install_media_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2638_marker[] = "libcgj-batch2638";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if path looks like a usable absolute install-media path:
 * leading '/', at least one more non-NUL character.
 */
static int
b2638_media_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '/') {
		return 0;
	}
	if (szPath[1] == '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_media_ok - soft check for an install media path.
 *
 * path: NUL-terminated absolute path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and has length >= 2, else 0.
 * Does not call libc or touch the filesystem.
 */
int
gj_install_media_ok(const char *szPath)
{
	(void)NULL;
	return b2638_media_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_install_media_ok(const char *szPath)
    __attribute__((alias("gj_install_media_ok")));
