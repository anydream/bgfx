#pragma once

#include "Utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////
int utf8_mbtowc(uint32_t *pwc, const uint8_t *s, uint32_t n);
int utf8_wctomb(uint8_t *r, uint32_t wc, uint32_t n);
int utf16_mbtowc(uint32_t *pwc, const uint16_t *s, uint32_t n);
int utf16_wctomb(uint16_t *r, uint32_t wc, uint32_t n);

uint32_t utf8to16(const char *strU8, uint32_t nU8, char16_t *strU16, uint32_t nU16, char16_t badChar);
uint32_t utf16to8(const char16_t *strU16, uint32_t nU16, char *strU8, uint32_t nU8, char badChar);

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
