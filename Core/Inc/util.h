#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "settings.h"

typedef enum {
    ERR_NO_ERR,
    ERR_CONFIG_INCOMPLETE,
    ERR_CONNECT_FAIL,
    ERR_INTERNAL_ERROR,
    ERR_TIMEOUT_ERROR,
    ERR_FATAL_ERROR,
} Err;

#define ATTR_RAM_FUNC  __attribute__((long_call, noinline, section(".data#")))
#define ATTR_SHARED  __attribute__((section(".shared")))
#define IS_ZERO(fp) (fpclassify(fp) == FP_ZERO)
#define IS_INFINITE(fp) (fpclassify(fp) == FP_INFINITE)
#define IS_NAN(fp) (fpclassify(fp) == FP_NAN)
#define MAX_UINT(a, b) ((a) > (b) ? a : b)
#define COUNTOF(a) (sizeof a / sizeof a[0U])
#define CPU_CYCLES_TO_LOOP_CYCLES(a) (a >> 1U)

#define DELAY_MICRO(micros) do { \
        uint32_t i = micros;    \
        while (i-- > 0) {       \
            asm("nop");         \
        }                       \
} while (0)

#define SECS_IN_MINUTE    (60U)
#define SET_BYTE(x, y, which)           ((x) |= ((y) << (which * 8)))
#define GET_BYTE(n, k)                  (((n) >> (k * 8)) & 0xFF)
#define CHECK_BIT(x,which)              ((x) &   (1<<(which)))


int map(int x, int in_min, int in_max, int out_min, int out_max);
uint8_t bcd2hex(uint8_t bcd);
uint8_t hex2bcd(uint8_t hex);
uint16_t bcd2hex16(uint16_t bcd);
uint16_t hex2bcd16(uint16_t hex);
uint32_t bcd2hex32(uint32_t bcd);
uint32_t hex2bcd32(uint32_t hex);
uint64_t tickdiff(uint64_t t1, uint64_t t2);
void memcpy_inram(void * __restrict__ dst, const void * __restrict__ src,
                  size_t cnt);

#ifndef __htonll
# if _BYTE_ORDER == _LITTLE_ENDIAN
# define __htonll(_x) __bswap64(_x)
# define __ntohll(_x) __bswap64(_x)
# else
# define __htonll(_x) ((__uint64_t)(_x))
# define __ntohll(_x) ((__uint64_t)(_x))
# endif
#endif /* __htonll */

#if defined(DEBUG) && (defined(USING_SEMIHOSTING) || PRINTF_DEBUG_ENABLED == 1)
#define DBG(format, ...) do { \
    printf(format, ## __VA_ARGS__); \
} while (0)
#else
#define DBG(format, ...) do { } while (0)
#endif

#if defined(USING_SEMIHOSTING) || PRINTF_DEBUG_ENABLED == 1
#define LOG(format, ...) do { \
    printf(format, ## __VA_ARGS__); \
} while (0)
#else
#define LOG(format, ...) do { } while (0)
#endif

#endif /* __UTIL_H__ */
