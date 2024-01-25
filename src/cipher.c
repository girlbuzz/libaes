#include <aes/aes.h>
#include "private.h"
#include <string.h>

void swap_col_row_cpy(uint8_t a[4][4], uint8_t b[16]) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            a[j][i] = b[(i * 4) + j];
        }
    }
}

/*
 * xTimes(b) = { {b6 b5 b4 b3 b2 b1 b0 0}                     , b7 = 0
 *             { {b6 b5 b4 b3 b2 b1 b0 0} ^ {0 0 0 1 1 0 1 1} , b7 = 1
 */
static uint8_t xTimes(uint8_t b) {
    return (b & 0x80) ? ((b << 1) ^ 0x1B) : (b << 1);
}

static uint8_t times3(uint8_t b) {
    return xTimes(b) ^ b;
}

static void SubBytes(state_t state) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            state[i][j] = Sbox[state[i][j]];
        }
    }
}

static void shift_once(uint8_t arr[4]) {
    uint8_t temp = arr[0];
    arr[0] = arr[1];
    arr[1] = arr[2];
    arr[2] = arr[3];
    arr[3] = temp;
}

static void shift(uint8_t arr[4], size_t amount) {
    for (size_t i = 0; i < amount; i++) {
        shift_once(arr);
    }
}

static void ShiftRows(state_t state) {
    shift(state[1], 1);
    shift(state[2], 2);
    shift(state[3], 3);
}

static void MixColumns(state_t state) {
    state_t statep;

    for (size_t c = 0; c < 4; c++) {
        statep[0][c] = xTimes(state[0][c]) ^ times3(state[1][c]) ^ state[2][c] ^ state[3][c];
        statep[1][c] = state[0][c] ^ xTimes(state[1][c]) ^ times3(state[2][c]) ^ state[3][c];
        statep[2][c] = state[0][c] ^ state[1][c] ^ xTimes(state[2][c]) ^ times3(state[3][c]);
        statep[3][c] = times3(state[0][c]) ^ state[1][c] ^ state[2][c] ^ xTimes(state[3][c]);
    }

    memcpy(state, statep, sizeof(state_t));
}

static void AddRoundKey(state_t state, uint32_t w[4]) {
    for (size_t c = 0; c < 4; c++) {
        uint32_t column = (uint32_t)state[3][c]
            | (((uint32_t)state[2][c]) << 8)
            | (((uint32_t)state[1][c]) << (8 * 2))
            | (((uint32_t)state[0][c]) << (8 * 3));

        column ^= w[c];

        state[3][c] = column & 0xFF;
        state[2][c] = (column >> 8) & 0xFF;
        state[1][c] = (column >> (8 * 2)) & 0xFF;
        state[0][c] = (column >> (8 * 3)) & 0xFF;
    }
}

void cipher(char in[16], const size_t Nr, uint32_t w[Nr + 1][4], state_t state) {
    swap_col_row_cpy(state, in);
    AddRoundKey(state, w[0]);

    for (size_t round = 1; round < Nr; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, w[round]);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, w[Nr]);
}
