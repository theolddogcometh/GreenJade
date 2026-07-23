/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1667: file-descriptor validity hint.
 *
 * Surface (unique symbols):
 *   int gj_fd_valid_hint(int fd);
 *     — Return 1 if fd >= 0 (non-negative descriptor slot), else 0.
 *       Pure local hint; does not call fcntl/fstat or probe the
 *       kernel. Negative values (including -1 closed/error) fail.
 *   int __gj_fd_valid_hint  (alias)
 *   __libcgj_batch1667_marker = "libcgj-batch1667"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1667_marker[] = "libcgj-batch1667";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_valid_hint — local non-negative fd predicate.
 *
 * "Hint" because a non-negative int may still be closed or unopened
 * in the process; this only screens the classic -1 / error encoding.
 */
int
gj_fd_valid_hint(int iFd)
{
	return (iFd >= 0) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fd_valid_hint(int iFd)
    __attribute__((alias("gj_fd_valid_hint")));
