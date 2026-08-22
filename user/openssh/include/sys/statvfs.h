/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/statvfs.h> for OpenSSH-portable (sftp-server ST_RDONLY).
 * Host POSIX: glibc. DUT: libcgj statvfs omits f_flag bits.
 */
#pragma once
#include_next <sys/statvfs.h>

#ifndef ST_RDONLY
# define ST_RDONLY 1
#endif
#ifndef ST_NOSUID
# define ST_NOSUID 2
#endif
#ifndef ST_NODEV
# define ST_NODEV 4
#endif
#ifndef ST_NOEXEC
# define ST_NOEXEC 8
#endif
#ifndef ST_SYNCHRONOUS
# define ST_SYNCHRONOUS 16
#endif
#ifndef ST_MANDLOCK
# define ST_MANDLOCK 64
#endif
#ifndef ST_NOATIME
# define ST_NOATIME 1024
#endif
#ifndef ST_NODIRATIME
# define ST_NODIRATIME 2048
#endif
#ifndef ST_RELATIME
# define ST_RELATIME 4096
#endif
