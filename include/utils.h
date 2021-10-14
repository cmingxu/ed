#ifndef UTILS_H
#define UTILS_H

void _pack_char_arr(char *, const char *, size_t);
void _pack_uint32(char *, uint32_t);
void _pack_int32(char *, int32_t);
void _pack_uint16(char *, uint16_t);
void _pack_short(char *, short);

void _debug_hex(void *, size_t);

char* int_to_ip(char *, uint16_t);
uint32_t ip_to_int(const char *ip);
uint32_t parse_packet_no(const char *header);
#endif
