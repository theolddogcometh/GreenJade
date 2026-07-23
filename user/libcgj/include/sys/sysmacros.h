/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/sysmacros.h. Not GNU glibc.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned int       major(dev_t dev);
unsigned int       minor(dev_t dev);
dev_t              makedev(unsigned int uMajor, unsigned int uMinor);
unsigned int       gnu_dev_major(unsigned long long uDev);
unsigned int       gnu_dev_minor(unsigned long long uDev);
unsigned long long gnu_dev_makedev(unsigned int uMajor, unsigned int uMinor);

#ifdef __cplusplus
}
#endif
