/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <pwd.h> for OpenSSH-portable. Host POSIX: glibc.
 * DUT: platform.c uses getspnam/struct spwd under USE_SHADOW without
 * including <shadow.h> (auth.h only pulls it with HAS_SHADOW_EXPIRE).
 */
#pragma once
#include_next <pwd.h>
#if !__STDC_HOSTED__
# include <shadow.h>
#endif
