/*************************************************************************************************
 * LOCATOR CJSC
 *
 * CONFIDENTIAL AND PROPRIETARY
 * FOR USE BY AUTHORIZED PERSONS ONLY
 *
 * This is an unpublished work fully protected by the copyright
 * laws and is a trade secret belonging to the copyright holder.
 *
 * Copyright (c) 2007-2021 LOCATOR CJSC. All Rights Reserved
 *************************************************************************************************
 * util.c
 ************************************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "main.h"
#include "util.h"

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t bcd2hex(uint8_t bcd)
{
    return (bcd & 0xF) + ((bcd & 0xF0) >> 4) * 10;
}

uint8_t hex2bcd(uint8_t hex)
{
    return ((hex / 10) << 4) + (hex % 10);
}

uint16_t bcd2hex16(uint16_t bcd)
{
    return (
        (uint16_t)bcd2hex((uint8_t)(bcd >> 0U) * 1U) +
        (uint16_t)bcd2hex((uint8_t)(bcd >> 8U)) * 100U
    );
}

uint16_t hex2bcd16(uint16_t hex)
{
    return (
        (uint16_t)hex2bcd((uint8_t)(hex % 100U)) |
        (uint16_t)hex2bcd((uint8_t)((hex / 100U) % 100U)) << 8U
    );
}

uint32_t bcd2hex32(uint32_t bcd)
{
    return (
        (uint32_t)bcd2hex((uint8_t)(bcd >> 0U) * 1U) +
        (uint32_t)bcd2hex((uint8_t)(bcd >> 8U)) * 100U +
        (uint32_t)bcd2hex((uint8_t)(bcd >> 16U)) * 10000U +
        (uint32_t)bcd2hex((uint8_t)(bcd >> 24U)) * 1000000U
    );
}

uint32_t hex2bcd32(uint32_t hex)
{
    return (
        (uint32_t)hex2bcd((uint8_t)(hex % 100U)) |
        (uint32_t)hex2bcd((uint8_t)((hex / 100U) % 100U)) << 8U |
        (uint32_t)hex2bcd((uint8_t)((hex / 10000U) % 100U)) << 16U |
        (uint32_t)hex2bcd((uint8_t)((hex / 1000000U) % 100U)) << 24U
    );
}

uint64_t tickdiff(uint64_t t1, uint64_t t2)
{
    return (t2 >= t1) ? t2 - t1 : UINT64_MAX - t1 + t2 + 1U;
}

void memcpy_inram(void * __restrict__ dst, const void * __restrict__ src,
                  size_t cnt)
{
    uint8_t *dst_u8 = (uint8_t *)dst;
    const uint8_t *src_u8 = (const uint8_t *)src;
    size_t i;

    for (i = 0U; i < cnt; i++) {
        dst_u8[i] = src_u8[i];
    }
}
