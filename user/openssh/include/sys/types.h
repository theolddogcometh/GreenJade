/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/types.h> for OpenSSH-portable. Host POSIX compiles
 * (#include_next glibc). BSD u_char / u_int* aliases if the next header
 * did not supply them (libcgj is POSIX scalars only).
 */
#pragma once

#include_next <sys/types.h>
#include <stdint.h>

#ifndef __u_char_defined
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
# define __u_char_defined
#endif
#ifndef __BIT_TYPES_DEFINED__
typedef uint8_t        u_int8_t;
typedef uint16_t       u_int16_t;
typedef uint32_t       u_int32_t;
typedef uint64_t       u_int64_t;
# define __BIT_TYPES_DEFINED__
#endif
#ifndef __GLIBC__
typedef unsigned long  fd_mask;
#endif
#ifndef _CADDR_T_DEFINED
typedef char *caddr_t;
# define _CADDR_T_DEFINED
#endif
