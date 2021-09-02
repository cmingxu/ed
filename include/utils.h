#ifndef UTILS_H
#define UTILS_H

void _pack_uint32(char *, uint32_t, size_t);
void _pack_int32(char *, int32_t, size_t);
void _pack_uint16(char *, uint16_t, size_t);
void _pack_short(char *, short, size_t);

int _send(char *, size_t size);
int _write(char *, size_t size);
int _recv(char *);
int _read(char *);

#endif
