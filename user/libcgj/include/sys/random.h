/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <sys/random.h> for libcgj (HAVE_SYS_RANDOM_H).
 * getrandom also lives in <unistd.h>. Dual DoD B OPEN.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GRND_NONBLOCK
#define GRND_NONBLOCK 0x0001u
#endif
#ifndef GRND_RANDOM
#define GRND_RANDOM   0x0002u
#endif
#ifndef GRND_INSECURE
#define GRND_INSECURE 0x0004u
#endif

ssize_t getrandom(void *pBuf, size_t cb, unsigned int uFlags);
int     getentropy(void *pBuf, size_t cb);

#ifdef __cplusplus
}
#endif
