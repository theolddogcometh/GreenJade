/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room sys/swap.h (swapon/swapoff).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP_FLAG_PREFER       0x8000
#define SWAP_FLAG_DISCARD      0x10000
#define SWAP_FLAG_DISCARD_ONCE 0x20000
#define SWAP_FLAG_DISCARD_PAGES 0x40000

int swapon(const char *szPath, int nFlags);
int swapoff(const char *szPath);

#ifdef __cplusplus
}
#endif
