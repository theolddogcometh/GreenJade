/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <features.h> for OpenSSH-portable (HAVE_FEATURES_H).
 * Host POSIX compiles (#include_next glibc). DUT -ffreestanding must
 * not pull glibc (would set __GLIBC__ and skip libcgj fd_mask).
 * Do not define extra __GLIBC__ here.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <features.h>
#endif
