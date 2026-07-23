/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX basic types (Linux-porter surface). No kernel caps exposed.
 *
 * Greppable product markers (also in README / init log):
 *   GREENJADE_UDX
 *   UDX_PRODUCT
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

/* Product identity — greppable in binaries and docs. */
#define UDX_PRODUCT_NAME    "GREENJADE_UDX"
#define UDX_PRODUCT_TAG     "UDX_PRODUCT"
#define UDX_PRODUCT_VERSION "1.2"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  s32;

typedef u64 udx_dma_addr_t;
typedef u64 udx_phys_addr_t;

/* Status: 0 success, negative error (Linux-ish). */
typedef int udx_status_t;

#define UDX_OK            0
#define UDX_ERR_INVAL    (-1)
#define UDX_ERR_NOMEM    (-2)
#define UDX_ERR_NODEV    (-3)
#define UDX_ERR_BUSY     (-4)
#define UDX_ERR_IO       (-5)
#define UDX_ERR_NOSYS    (-6)
#define UDX_ERR_NODEVIRQ (-7)
#define UDX_ERR_NODMA    (-8)
#define UDX_ERR_AGAIN    (-9)

#ifndef NULL
#define NULL ((void *)0)
#endif
