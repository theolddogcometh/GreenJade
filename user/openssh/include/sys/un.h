/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/un.h> for OpenSSH-portable (HAVE_SYS_UN_H).
 * Host POSIX compiles (#include_next glibc).
 */
#pragma once
#include_next <sys/un.h>

#ifndef UNIX_PATH_MAX
# define UNIX_PATH_MAX 108
#endif
#ifndef SUN_LEN
# include <string.h>
# define SUN_LEN(pSu) \
    ((size_t)(((struct sockaddr_un *)0)->sun_path) + strlen((pSu)->sun_path))
#endif
