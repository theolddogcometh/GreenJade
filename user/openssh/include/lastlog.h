/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <lastlog.h> for OpenSSH-portable (HAVE_LASTLOG_H).
 * Host POSIX compiles (#include_next glibc; struct lastlog via utmp).
 */
#pragma once
#include_next <lastlog.h>
