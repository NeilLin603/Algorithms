#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define __TMS320C2000__ // C2000 data type

#define CRC_POLYNOMIAL1 0xA001 // 0x8005 reversed: CRC-16-IBM   (SL1 Rx / SL2 Tx)
#define CRC_POLYNOMIAL2 0x8408 // 0x1021 reversed: CRC-16-CCITT (SL1 Tx / SL2 Rx)

#if defined(__TMS320C2000__)
static uint16_t crcRem(const void *base, size_t wordSize, uint16_t poly) {
    uint16_t rem = *(uint16_t *)base, wordCount = 0, bitCount, next;
    while (++wordCount < wordSize) {
        next = *((uint16_t *)base + wordCount);
        for (bitCount = 0; bitCount < 16; bitCount++) {
            rem = (rem >> 1 | (next & 1) << 15) ^ (rem & 1 ? poly : 0);
            next >>= 1;
        }
    }
    return rem;
}

/**
 * \brief 16-bit CRC encoder.
 * \param base Pointer to the base address of data to be encoded.
 * \param wordSize Data size in words to be encoded.
 * \param poly The CRC polynomial.
 * \return 16-bit CRC.
 */
uint16_t encodeCRC16(const void *base, size_t wordSize, uint16_t poly) {
    uint16_t crc = crcRem(base, wordSize, poly), bitCount;
    for (bitCount = 0; bitCount < 16; bitCount++) {
        crc = (crc >> 1) ^ (crc & 1 ? poly : 0);
    }
    return crc;
}

/**
 * \brief 16-bit CRC decoder.
 * \param base Pointer to the base address of data to be encoded.
 * \param wordSize Data size in words to be encoded.
 * \param poly The CRC polynomial.
 * \param crc CRC value for verification.
 * \return true: check passed, false: check failed.
 */
bool decodeCRC16(const void *base, size_t wordSize, uint16_t poly, uint16_t crc) {
    uint16_t rem = crcRem(base, wordSize, poly), bitCount;
    for (bitCount = 0; bitCount < 16; bitCount++) {
        rem = (rem >> 1 | (crc & 1) << 15) ^ (rem & 1 ? poly : 0);
        crc >>= 1;
    }
    return !rem;
}
#else
/**
 * \brief 16-bit CRC encoder.
 * \param base Pointer to the base address of data to be encoded.
 * \param size Data size in bytes to be encoded.
 * \param poly The CRC polynomial.
 * \return 16-bit CRC.
 */
uint16_t encodeCRC16(const void *base, size_t size, uint16_t poly) {
    uint16_t crc = size > 1 ? *(uint16_t *)base : (uint16_t)*(char *)base;
    uint16_t byteCount = 1, bitCount, next;
    while (byteCount++ < size + 1) {
        next = byteCount < size ? (uint16_t)*((char *)base + byteCount) : 0;
        for (bitCount = 0; bitCount < 8; bitCount++) {
            crc = (crc >> 1 | (next & 1) << 15) ^ (crc & 1 ? poly : 0);
            next >>= 1;
        }
    }
    return crc;
}

/**
 * \brief 16-bit CRC decoder.
 * \param base Pointer to the base address of data to be encoded.
 * \param size Data size in bytes to be encoded.
 * \param poly The CRC polynomial.
 * \param crc CRC value for verification.
 * \return true: check passed, false: check failed.
 */
bool decodeCRC16(const void *base, size_t size, uint16_t poly, uint16_t crc) {
    uint16_t rem = size > 1 ? *(uint16_t *)base : (uint16_t)*(char *)base | (crc & 0xff) << 8;
    uint16_t byteCount = 1, bitCount, next;
    while (byteCount++ < size + 1) {
        if (byteCount < size) {
            next = (uint16_t)*((char *)base + byteCount);
        } else if (byteCount == size) {
            next = crc & 0xff;
        } else {
            next = crc >> 8;
        }
        for (bitCount = 0; bitCount < 8; bitCount++) {
            rem = (rem >> 1 | (next & 1) << 15) ^ (rem & 1 ? poly : 0);
            next >>= 1;
        }
    }
    return !rem;
}
#endif /* __TMS320C2000__ */

int main() {
    uint16_t u16Nums[] = {0x1234, 0x5678, 0x9453, 0x3064}, u16Crc;
    uint8_t *u8Nums = (uint8_t *)malloc(sizeof(u16Nums));
#if defined(__TMS320C2000__)
    size_t u16NumsSize = sizeof(u16Nums) / sizeof(uint16_t);
#else
    size_t u16NumsSize = sizeof(u16Nums);
#endif /* __TMS320C2000__ */

    // Copy memory from u16Nums to u8Nums
    memcpy(u8Nums, u16Nums, sizeof(u16Nums));

    // Encode CRC for u16Nums
    u16Crc = encodeCRC16(u16Nums, u16NumsSize, CRC_POLYNOMIAL1);
    printf("CRC(u16Nums) = 0x%x\n", u16Crc);

    // Decode CRC for u16Nums
    if (decodeCRC16(u16Nums, u16NumsSize, CRC_POLYNOMIAL1, u16Crc)) {
        printf("CRC test for u16Nums passed.\n");
    } else {
        printf("CRC test for u16Nums failed.\n");
    }

#if !defined(__TMS320C2000__)
    // Encode CRC for u8Nums
    u16Crc = encodeCRC16(u8Nums, sizeof(u8Nums), CRC_POLYNOMIAL1);
    printf("CRC(u8Nums)  = 0x%x\n", u16Crc);

    // Decode CRC for u8Nums
    if (decodeCRC16(u8Nums, sizeof(u8Nums), CRC_POLYNOMIAL1, u16Crc)) {
        printf("CRC test for u8Nums passed.\n");
    } else {
        printf("CRC test for u8Nums failed.\n");
    }
#endif /* __TMS320C2000__ */

    free(u8Nums);
    return 0;
}
