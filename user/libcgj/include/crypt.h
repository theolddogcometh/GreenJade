/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room crypt.h stub surface. Not GNU libcrypt.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

char *crypt(const char *szKey, const char *szSalt);
char *crypt_r(const char *szKey, const char *szSalt, void *pData);

#ifdef __cplusplus
}
#endif
