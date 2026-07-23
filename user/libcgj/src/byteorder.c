/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * htonl/htons/ntohl/ntohs as exported symbols (host-dynlink shape).
 */
#include <stdint.h>

uint16_t
htons(uint16_t v)
{
    return (uint16_t)((v << 8) | (v >> 8));
}

uint16_t
ntohs(uint16_t v)
{
    return htons(v);
}

uint32_t
htonl(uint32_t v)
{
    return ((v & 0xffu) << 24) | ((v & 0xff00u) << 8) |
           ((v & 0xff0000u) >> 8) | ((v >> 24) & 0xffu);
}

uint32_t
ntohl(uint32_t v)
{
    return htonl(v);
}
