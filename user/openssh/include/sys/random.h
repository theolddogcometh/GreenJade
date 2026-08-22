/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/random.h> for OpenSSH-portable (HAVE_SYS_RANDOM_H).
 * Host POSIX: glibc. DUT: libcgj has no sys/random.h; do not include_next
 * glibc (uses __wur and breaks libcgj stdlib.h). getrandom lives in unistd.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <sys/random.h>
#else
#include <stddef.h>
#include <sys/types.h>
#ifndef GRND_NONBLOCK
# define GRND_NONBLOCK 0x0001
#endif
#ifndef GRND_RANDOM
# define GRND_RANDOM 0x0002
#endif
#ifndef GRND_INSECURE
# define GRND_INSECURE 0x0004
#endif
ssize_t getrandom(void *pBuf, size_t cb, unsigned int uFlags);
int     getentropy(void *pBuf, size_t cb);
#endif
