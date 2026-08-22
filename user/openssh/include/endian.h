/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <endian.h> for OpenSSH-portable (HAVE_ENDIAN_H,
 * HAVE_DECL_HTOBE32 / HTOLE64 / LE32TOH / LE64TOH).
 * Host POSIX compiles (#include_next glibc).
 */
#pragma once
#include_next <endian.h>

#ifndef __LITTLE_ENDIAN
# define __LITTLE_ENDIAN 1234
#endif
#ifndef __BIG_ENDIAN
# define __BIG_ENDIAN 4321
#endif
#ifndef __PDP_ENDIAN
# define __PDP_ENDIAN 3412
#endif
#ifndef __BYTE_ORDER
# define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#ifndef LITTLE_ENDIAN
# define LITTLE_ENDIAN __LITTLE_ENDIAN
#endif
#ifndef BIG_ENDIAN
# define BIG_ENDIAN __BIG_ENDIAN
#endif
#ifndef PDP_ENDIAN
# define PDP_ENDIAN __PDP_ENDIAN
#endif
#ifndef BYTE_ORDER
# define BYTE_ORDER __BYTE_ORDER
#endif

#ifndef htobe16
# include <stdint.h>
# define htobe16(x) ((uint16_t)__builtin_bswap16((uint16_t)(x)))
# define htobe32(x) ((uint32_t)__builtin_bswap32((uint32_t)(x)))
# define htobe64(x) ((uint64_t)__builtin_bswap64((uint64_t)(x)))
# define htole16(x) ((uint16_t)(x))
# define htole32(x) ((uint32_t)(x))
# define htole64(x) ((uint64_t)(x))
# define be16toh(x) htobe16(x)
# define be32toh(x) htobe32(x)
# define be64toh(x) htobe64(x)
# define le16toh(x) htole16(x)
# define le32toh(x) htole32(x)
# define le64toh(x) htole64(x)
#endif
