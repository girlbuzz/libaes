#ifndef __AES_H
#define __AES_H

#include <stddef.h>
#include <inttypes.h>

typedef uint32_t AES128Key[4];

void aes128_encrypt(void *data, size_t size, const AES128Key key);

#endif /* __AES_H */