/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <fcntl.h> for OpenSSH-portable (sftp-server O_ACCMODE).
 * Host POSIX: glibc. DUT: libcgj fcntl.h omits O_ACCMODE.
 */
#pragma once
#include_next <fcntl.h>

#ifndef O_ACCMODE
# define O_ACCMODE 3
#endif
