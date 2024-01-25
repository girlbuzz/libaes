#include <aes/aes.h>
#include "private.h"
#include <string.h>

#define Nr (10)

static uint32_t Rcon[10] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1b000000, 0x36000000
};

static uint32_t RotWord(uint32_t word) {
    return word << 8 | word >> 24;
}

static uint32_t SubWord(uint32_t word) {
    uint8_t temp[4] = {
        word & 0xFF,
        (word >> 8) & 0xFF,
        (word >> (8 * 2)) & 0xFF,
        (word >> (8 * 3)) & 0xFF,
    };

    temp[0] = Sbox[temp[0]];
    temp[1] = Sbox[temp[1]];
    temp[2] = Sbox[temp[2]];
    temp[3] = Sbox[temp[3]];

    return *(uint32_t*) temp;
}

static void KeyExpansion(const AES128Key key, uint32_t w[4 * (Nr + 1)]) {
    size_t i = 0;
    const size_t Nk = sizeof(AES128Key) / sizeof(uint32_t);

    while (i < Nk) {
        w[i] = key[i];
        i++;
    }

    while (i <= 4 * Nr + 3) {
        uint32_t temp = w[i - 1];

        if (i % Nk == 0) {
            temp = SubWord(RotWord(temp)) ^ Rcon[(i / Nk) - 1];
        } else if (Nk > 6 && i % Nk == 4) {
            temp = SubWord(temp);
        }

        w[i] = w[i - Nk] ^ temp;
        i++;
    }
}

void aes128_encrypt(void *data, size_t nbytes, const AES128Key key) {
    char overflow[16] = { 0 };
    size_t remaining = nbytes % 16;
    uint32_t w[4 * (Nr + 1)];
    state_t state;
    size_t i;

    KeyExpansion(key, w);

    for (i = 0; i < nbytes / 16; i++) {
        cipher((char*) data + i * 16, Nr, w, state);
        swap_col_row_cpy((char*) data + i * 16, state);
    }

    if (remaining == 0) return;

    memcpy(overflow, data, remaining);
    cipher(overflow, Nr, w, state);

    swap_col_row_cpy(overflow, state);
    memcpy((char*) data + i * 16, overflow, remaining);
}
