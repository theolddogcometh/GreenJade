/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Tiny kernel heap (freelist over PMM pages, HHDM-mapped).
 * Allocs zeroed; free scrubs payload. Max single alloc < one page payload.
 */
#pragma once

#include <gj/types.h>

void  kheap_init(void);

/** Zeroed allocation; NULL on OOM or oversized request. */
void *kheap_alloc(size_t cb);

/** Free; safe on NULL; double-free is a no-op (magic guard). */
void  kheap_free(void *p);

/** Bytes currently charged to live payloads (approx). */
size_t kheap_used_bytes(void);
