/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/stat.h> for OpenSSH-portable (sftp-common st_atime/st_mtime).
 * Host POSIX: glibc. DUT: libcgj uses st_atim[2] without POSIX aliases.
 */
#pragma once
#include_next <sys/stat.h>

#if !__STDC_HOSTED__
# ifndef st_atime
#  define st_atime st_atim[0]
# endif
# ifndef st_mtime
#  define st_mtime st_mtim[0]
# endif
# ifndef st_ctime
#  define st_ctime st_ctim[0]
# endif
#endif
