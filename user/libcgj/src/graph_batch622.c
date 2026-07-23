/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch622: Jenkins one-at-a-time hash (audit / marker).
 *
 * Requested surface:
 *   uint32_t gj_jenkins_oaat(const void *data, size_t n);
 *     — Bob Jenkins one-at-a-time (OAAT) over n bytes at data. Seed 0;
 *       per-byte: h += c; h += h << 10; h ^= h >> 6; then avalanche:
 *       h += h << 3; h ^= h >> 11; h += h << 15.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   __gj_jenkins_oaat  (alias)
 *
 * Already present elsewhere (NOT redefined here — avoid multi-def):
 *   gj_jenkins_oaat / __gj_jenkins_oaat  → graph_batch456.c
 *   (also listed in libc.map under batch456)
 *
 * This TU only exports the batch identity marker so link-time presence of
 * batch622 can be probed without colliding with batch456's Jenkins OAAT.
 *   __libcgj_batch622_marker = "libcgj-batch622"
 *
 * Clean-room freestanding pure C. Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

const char __libcgj_batch622_marker[] = "libcgj-batch622";
