#include "ed.h"
#include "utils.h"

void _pack_uint32(char *pos, uint32_t value){
  size_t size = 4;
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> (i * 8) & 0xFF;
  }
}

void _pack_int32(char *pos, int32_t value){
  size_t size = 4;
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> (i * 8) & 0xFF;
  }
}

void _pack_uint16(char *pos, uint16_t value){
  size_t size = 2;
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> (i * 8) & 0xFF;
  }
}

void _pack_short(char *pos, short value){
  size_t size = 1;
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> (i * 8) & 0xFF;
  }
}

void _pack_char_arr(char *target, const char *src, size_t size){
  for (int i = 0; i < size; i++) {
    *(target+i) = src[size-i-1];
  }
}

void _debug_hex(void *buf, size_t size){
#ifdef ED_DEBUG
  char *tmp = (char *)buf;
  for (int i = 0; i < size; i++) {
    if ((i % 8 == 0)) {
      printf("\n");
    }
    printf("%02X ",  *(tmp+i) & 0xff);
  }

  printf("\n");
#endif
}


uint32_t parse_packet_no(const char *header) {
	return 	((header[3] & 0xff) << 24) +
		((header[2] & 0xff) << 16) +
		((header[1] & 0xff) << 8) +
		(header[0] & 0xff);
}


char* int_to_ip(char *buffer, uint16_t ip){
	sprintf(buffer, "%d.%d.%d.%d",
			(ip >> 24) & 0xFF,
			(ip >> 16) & 0xFF,
			(ip >>  8) & 0xFF,
			(ip      ) & 0xFF);

	return buffer;
}

uint32_t ip_to_int(const char *ip){
	int32_t v = 0;
	int i;
	const char * start;

	start = ip;
	for (i = 0; i < 4; i++) {
		char c;
		int n = 0;
		while (1) {
			c = * start;
			start++;
			if (c >= '0' && c <= '9') {
				n *= 10;
				n += c - '0';
			}
			else if ((i < 3 && c == '.') || i == 3) {
				break;
			}
			else {
				return 0;
			}
		}
		if (n >= 256) {
			return 0;
		}
		v *= 256;
		v += n;
	}
	return v;
}
