#include "ed.h"
#include "util.h"

void _pack_uint32(char *pos, uint32_t value, size_t size){
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> ((size - 1 - i) * 8) & 0xFF;
  }
}

void _pack_int32(char *pos, int32_t value, size_t size){
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> ((size - 1 - i) * 8) & 0xFF;
  }
}

void _pack_uint16(char *pos, uint16_t value, size_t size){
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> ((size - 1 - i) * 8) & 0xFF;
  }
}

void _pack_short(char *pos, short value, size_t size){
  for (int i = 0; i < size; i++) {
    *(pos+i) = value >> ((size - 1 - i) * 8) & 0xFF;
  }
}
