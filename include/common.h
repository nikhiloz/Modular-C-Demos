/**
 * @file common.h
 * @brief Common header file with shared macros, types, and utilities
 */

#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>

/* Version Info */
#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_STRING "2.0.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

/* Utility Macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MIN(MAX(x, lo), hi)
#define UNUSED(x) (void)(x)
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/* Bit Macros */
#define BIT(n)              (1UL << (n))
#define SET_BIT(x, n)       ((x) |= BIT(n))
#define CLEAR_BIT(x, n)     ((x) &= ~BIT(n))
#define TOGGLE_BIT(x, n)    ((x) ^= BIT(n))
#define CHECK_BIT(x, n)     (((x) >> (n)) & 1)

/* Memory Size Macros */
#define KB(x) ((x) * 1024UL)
#define MB(x) ((x) * 1024UL * 1024UL)

/* Type Definitions */
typedef enum {
    RESULT_OK = 0,
    RESULT_ERROR = -1,
    RESULT_INVALID_PARAM = -2,
    RESULT_NO_MEMORY = -3
} result_t;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Section Display Macros */
#define DEMO_SECTION(title) \
    printf("\n========================================\n"); \
    printf("  %s\n", title); \
    printf("========================================\n\n")

#define DEMO_SUBSECTION(title) \
    printf("\n--- %s ---\n\n", title)

#define DEMO_END() \
    printf("\n----------------------------------------\n")

#endif /* COMMON_H */
