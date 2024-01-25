
OBJS=src/aes128.o src/cipher.o

CFLAGS += -Iinclude

.PHONY: all clean

all: libaes.so libaes.a

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

libaes.so: $(OBJS)
	$(LD) $(LDFLAGS) -shared -o $@ $(OBJS)

libaes.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	$(RM) $(OBJS) libaes.so libaes.a
