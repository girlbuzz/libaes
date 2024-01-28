#include <stdio.h>
#include <string.h>
#include <aes/aes.h>

static char tohex[] = {
    '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f',
};

void printhex(const char *data, size_t n) {
    uint16_t *wdata = (uint16_t*)data;

    for (size_t i = 0; i < n; i++) {
        unsigned char c = data[i];

        putchar(tohex[(c & 0xF0) >> 4]);
        putchar(tohex[c & 0x0F]);
    }

    putchar('\n');
}

int main() {
    /* this key might not work right on a big endian system.
     * the algorithm never uses the key at a byte level, so it shouldn't be fine. However, know that internally the bytes
     * aren't in the proper order. To export or something you will need to convert to big endian. */
    AES128Key key = { 0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c };
    char plaintext[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};

    printf("key: ");
    printf("%x%x%x%x\n", key[0], key[1], key[2], key[3]);

    printf("plaintext: ");
    printhex(plaintext, sizeof(plaintext));

    aes128_encrypt(plaintext, sizeof(plaintext), key);

    printf("ciphertext: ");
    printhex(plaintext, sizeof(plaintext));
}
