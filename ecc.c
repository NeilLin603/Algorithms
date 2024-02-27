#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// Define decodeECC() return form
#define DECODER_VERSION 1 // 1: returns true/false, 2: returns bit position

// Defines for functional test
#define TEST_CASE           1    // 1: Test specific error bit(s) injection, 2: Test all single error bit injection
#define TEST_DATA_LENGTH    1023 // 1 to 1023
#if (TEST_CASE == 1)
    #define ERROR_INDEX     87   // 0 to (TEST_DATA_LENGTH - 1)
    #define ERROR_BIT       3    // 0 to 15
#endif

/**
 * \brief Re-arrange data bits from \b non-systematic form to \b systematic form based on extended Hamming code.
 * \param nsysData Pointer to the non-systematic form data to be systematized.
 * \param data Pointer to the array to store the systematic form data.
 * \param wordSize The \e data size in words (16-bit).
 * \return Parity.
 */
uint16_t hammingSystematize(const uint16_t *nsysData, uint16_t *data, size_t wordSize) {
    uint16_t parity = nsysData[0] & 1, nsysBitPos = 0, dataBitPos = 0, temp;
    for (temp = 0; temp < wordSize; temp++) {
        data[temp] = 0;
    }
    temp = 1;
    while (dataBitPos < wordSize << 4) {
        if (++nsysBitPos == temp) { /* Parity bits */
            parity |= (nsysData[nsysBitPos >> 4] >> nsysBitPos % 16 & 1) << nsysBitPos - dataBitPos;
            temp <<= 1;
        } else { /* Data bits */
            data[dataBitPos >> 4] |= (nsysData[nsysBitPos >> 4] >> nsysBitPos % 16 & 1) << dataBitPos % 16;
            dataBitPos++;
        }
    }
    return parity;
}

/**
 * \brief Re-arrange data bits from \b systematic form to \b non-systematic form based on extended Hamming code.
 * \param nsysData Pointer to the array to store the non-systematic form data (array elements must be initialize to zeros before function calls).
 * \param data Pointer to the data (not including parity bits) to be unsystematized.
 * \param wordSize The \e data size in words (16-bit).
 * \param parity Parity.
 * \return Total bits of the non-systematic data (including data bits and parity bits).
 */
uint16_t hammingUnsystematize(uint16_t *nsysData, const uint16_t *data, size_t wordSize, uint16_t parity) {
    uint16_t nsysBitPos = 0, dataBitPos = 0, temp = 1;
    nsysData[0] |= parity & 1; // p0
    while (dataBitPos < wordSize << 4) {
        if (++nsysBitPos == temp) { /* Parity bits */
            nsysData[nsysBitPos >> 4] |= (parity >> nsysBitPos - dataBitPos & 1) << nsysBitPos % 16;
            temp <<= 1;
        } else { /* Data bits */
            nsysData[nsysBitPos >> 4] |= (data[dataBitPos >> 4] >> dataBitPos % 16 & 1) << nsysBitPos % 16;
            dataBitPos++;
        }
    }
    return ++nsysBitPos;
}

/**
 * \brief ECC encoder of Extended Hamming code algorithm.
 * \param startAddr Start address of the data to be encoded.
 * \param wordSize Data size in words (16-bit) to be encoded. The maximum value of \e wordSize is 2047.
 * \return 16-bit ECC (parity).
 */
uint16_t encodeECC(const uint16_t *startAddr, size_t wordSize) {
    if (wordSize > 2047) {
        return 0;
    }
    uint16_t *nsysData = (uint16_t *)calloc(wordSize + 1, sizeof(uint16_t)), parity = 0;
    uint16_t totalBits = hammingUnsystematize(nsysData, startAddr, wordSize, parity), bitPos = totalBits;
    while (--bitPos) {
        if (nsysData[bitPos >> 4] & 1 << bitPos % 16) {
            parity ^= bitPos << 1 | 1;
        }
    }
    free(nsysData);
    bitPos = totalBits - (wordSize << 4);
    while (--bitPos) {
        if (parity & 1 << bitPos) {
            parity ^= 1;
        }
    }
    return parity;
}


#if (DECODER_VERSION == 1)
/**
 * \brief ECC decoder of Extended Hamming code algorithm.
 * \param startAddr Start address of the data to be decoded.
 * \param wordSize Data size in words (16-bit) to be decoded. The maximum value of \e wordSize is 2047.
 * \param ecc Pointer to the corresponding ECC (parity).
 * \return \b true  - no error or single error which has been corrected,
 *         \b false - double error detected or the \e wordSize is too large to be decoded.
 */
bool decodeECC(uint16_t *startAddr, size_t wordSize, uint16_t *ecc) {
    if (wordSize > 2047) {
        return false;
    }
    uint16_t *nsysData = (uint16_t *)calloc(wordSize + 1, sizeof(uint16_t)), syndrome = 0;
    uint16_t bitPos = hammingUnsystematize(nsysData, startAddr, wordSize, *ecc);
    while (bitPos--) {
        if (nsysData[bitPos >> 4] & 1 << bitPos % 16) {
            syndrome ^= bitPos << 1 | 1;
        }
    }
    if (syndrome & 1) { /* Single error */
        nsysData[syndrome >> 5] ^= 1 << (syndrome >> 1) % 16; // Correct the error bit
    } else { /* Double error or no error */
        if (syndrome >> 1) { /* Double error */
            free(nsysData);
            return false;
        }
    }
    *ecc = hammingSystematize(nsysData, startAddr, wordSize);
    free(nsysData);
    return true;
}
#elif (DECODER_VERSION == 2)
/**
 * \brief ECC decoder of Extended Hamming code algorithm.
 * \param startAddr Start address of the data to be decoded.
 * \param wordSize Data size in words (16-bit) to be decoded. The maximum value of \e wordSize is 2047.
 * \param ecc Pointer to the corresponding ECC (parity).
 * \return The error bit position start from \e startAddr, the first bit position number is 1.
 *         Return 0 indicates no error. Return 1 to \e wordSize * 16 indicates single data error which has been corrected.
 *         Return \e wordSize * 16 + 1 to \e wordSize * 16 + 16 indicates single \e ecc error which has been corrected.
 *         Return 0xffff indicates double error detected or the \e wordSize is too large to be decoded.
 */
uint16_t decodeECC(uint16_t *startAddr, size_t wordSize, uint16_t *ecc) {
    if (wordSize > 2047) {
        return 0xffff;
    }
    uint16_t *nsysData = (uint16_t *)calloc(wordSize + 1, sizeof(uint16_t)), syndrome = 0, i = 0;
    uint16_t bitPos = hammingUnsystematize(nsysData, startAddr, wordSize, *ecc);
    while (bitPos--) {
        if (nsysData[bitPos >> 4] & 1 << bitPos % 16) {
            syndrome ^= bitPos << 1 | 1;
        }
    }
    if (syndrome & 1) { /* Single error */
        bitPos = syndrome >> 1;
        nsysData[bitPos >> 4] ^= 1 << bitPos % 16; // Correct the error bit
        while (bitPos) {
            bitPos >>= 1;
            i++;
        }
        bitPos = syndrome != (1 << i | 1) ? (syndrome >> 1) - i : (wordSize << 4) + i + 1; // data bit : parity bit
    } else { /* Double error or no error */
        if (syndrome >> 1) { /* Double error */
            free(nsysData);
            return 0xffff;
        }
        bitPos = 0;
    }
    *ecc = hammingSystematize(nsysData, startAddr, wordSize);
    free(nsysData);
    return bitPos;
}
#else
    #error "DECODER_VERSION undefined."
#endif /* DECODER_VERSION */

/**
 * \brief The main program.
 */
int main(void) {
    uint16_t data[TEST_DATA_LENGTH], wordSize = sizeof(data) / sizeof(data[0]), ecc;

    // Initialize data to random
    srand(time(NULL));
    for (int i = 0; i < TEST_DATA_LENGTH; i++) {
        data[i] = rand();
    }

    // Encode ECC
    ecc = encodeECC(data, wordSize);

#if (TEST_CASE == 1)
    uint16_t error = 1 << ERROR_BIT;
    printf("\nWord size of data = %d\n", wordSize);

    printf("**************** Original data ****************\n");
    printf("data[%d] = 0x%x\n", ERROR_INDEX, data[ERROR_INDEX]);
    printf("ecc = 0x%x\n", ecc);

    // Inject error
    data[ERROR_INDEX] ^= error;
    // ecc ^= error;
    printf("**************** Error inject: e = 0x%x ****************\n", error);
    printf("data[%d] = 0x%x\n", ERROR_INDEX, data[ERROR_INDEX]);
    printf("ecc = 0x%x\n", ecc);

    // Decode ECC
#if (DECODER_VERSION == 1)
    if (!decodeECC(data, wordSize, &ecc)) {
#elif (DECODER_VERSION == 2)
    uint16_t errorBit = decodeECC(data, wordSize, &ecc);
    if (!errorBit) {
        printf("**************** No error detected ****************\n");
    } else if (errorBit == 0xffff) {
#else
    #error "DECODER_VERSION undefined."
#endif /* DECODER_VERSION */
        printf("**************** Double error detected ****************\n");
    } else {
        printf("**************** Single error corrected ****************\n");
    }
    printf("data[%d] = 0x%x\n", ERROR_INDEX, data[ERROR_INDEX]);
    printf("ecc = 0x%x\n", ecc);
#if (DECODER_VERSION == 2)
    printf("Error bit position = %d\n", errorBit);
#endif /* DECODER_VERSION */
#elif (TEST_CASE == 2)
    uint16_t retData, retEcc = ecc, errBit;

    // Test for all data bit positions with single bit error injection
    for (errBit = 0; errBit < TEST_DATA_LENGTH << 4; errBit++) {
        retData = data[errBit >> 4];
        data[errBit >> 4] ^= 1 << errBit % 16;
        decodeECC(data, wordSize, &ecc);
        if (retData != data[errBit >> 4] || retEcc != ecc) {
            break;
        }
    }
    if (errBit == TEST_DATA_LENGTH << 4) {
        printf("Single data bit error injection test passed.\n");
    } else {
        printf("Single data bit error injection test failed.\n");
        printf("errBit = %d\n", errBit);
    }

    // Test for all parity bit positions with single bit error injection
    for (errBit = 0; errBit < 16; errBit++) {
        ecc ^= 1 << errBit;
        decodeECC(data, wordSize, &ecc);
        if (retEcc != ecc) {
            break;
        }
    }
    if (errBit == 16) {
        printf("Single parity bit error injection test passed.\n");
    } else {
        printf("Single parity bit error injection test failed.\n");
        printf("errBit = %d\n", errBit);
    }
#else
    printf("Test case undefined.\n");
#endif /* TEST_CASE */

    return 0;
}
