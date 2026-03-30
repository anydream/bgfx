#include "Encoding.h"

//////////////////////////////////////////////////////////////////////////
// Return code if invalid. (xxx_wctomb)
#define RET_ILUNI      (-1)
// Return code if output buffer is too small. (xxx_wctomb, xxx_reset)
#define RET_TOOSMALL   (-2)

// Return code if invalid input after a shift sequence of n bytes was read. (xxx_mbtowc)
#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
// Return code if invalid. (xxx_mbtowc)
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
// Return code if only a shift sequence of n bytes was read. (xxx_mbtowc)
#define RET_TOOFEW(n)       (-2-2*(n))

//////////////////////////////////////////////////////////////////////////
int utf8_mbtowc(uint32_t *pwc, const uint8_t *s, uint32_t n)
{
	uint8_t c = s[0];

	if (c < 0x80)
	{
		*pwc = c;
		return 1;
	}
	else if (c < 0xc2)
	{
		return RET_ILSEQ;
	}
	else if (c < 0xe0)
	{
		if (n < 2)
			return RET_TOOFEW(0);
		if (!((s[1] ^ 0x80) < 0x40))
			return RET_ILSEQ;
		*pwc = ((uint32_t)(c & 0x1f) << 6)
				| (uint32_t)(s[1] ^ 0x80);
		return 2;
	}
	else if (c < 0xf0)
	{
		if (n < 3)
			return RET_TOOFEW(0);
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (c >= 0xe1 || s[1] >= 0xa0)
			&& (c != 0xed || s[1] < 0xa0)))
			return RET_ILSEQ;
		*pwc = ((uint32_t)(c & 0x0f) << 12)
				| ((uint32_t)(s[1] ^ 0x80) << 6)
				| (uint32_t)(s[2] ^ 0x80);
		return 3;
	}
	else if (c < 0xf8 && sizeof(uint32_t) * 8 >= 32)
	{
		if (n < 4)
			return RET_TOOFEW(0);
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (s[3] ^ 0x80) < 0x40
			&& (c >= 0xf1 || s[1] >= 0x90)
			&& (c < 0xf4 || (c == 0xf4 && s[1] < 0x90))))
			return RET_ILSEQ;
		*pwc = ((uint32_t)(c & 0x07) << 18)
				| ((uint32_t)(s[1] ^ 0x80) << 12)
				| ((uint32_t)(s[2] ^ 0x80) << 6)
				| (uint32_t)(s[3] ^ 0x80);
		return 4;
	}
	else
		return RET_ILSEQ;
}

int utf8_wctomb(uint8_t *r, uint32_t wc, uint32_t n) // n == 0 is acceptable
{
	uint32_t count;
	if (wc < 0x80)
		count = 1;
	else if (wc < 0x800)
		count = 2;
	else if (wc < 0x10000)
	{
		if (wc < 0xd800 || wc >= 0xe000)
			count = 3;
		else
			return RET_ILUNI;
	}
	else if (wc < 0x110000)
		count = 4;
	else
		return RET_ILUNI;
	if (n < count)
		return RET_TOOSMALL;
	switch (count)
	{
	// note: code falls through cases!
	case 4: r[3] = (uint8_t)(0x80 | (wc & 0x3f));
		wc = wc >> 6;
		wc |= 0x10000;
	case 3: r[2] = (uint8_t)(0x80 | (wc & 0x3f));
		wc = wc >> 6;
		wc |= 0x800;
	case 2: r[1] = (uint8_t)(0x80 | (wc & 0x3f));
		wc = wc >> 6;
		wc |= 0xc0;
	case 1: r[0] = (uint8_t)(wc);
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////
int utf16_mbtowc(uint32_t *pwc, const uint16_t *s, uint32_t n)
{
	if (n >= 1)
	{
		uint32_t wc = s[0];
		if (wc >= 0xd800 && wc < 0xdc00)
		{
			if (n >= 2)
			{
				uint32_t wc2 = s[1];
				if (!(wc2 >= 0xdc00 && wc2 < 0xe000))
					goto ilseq;
				*pwc = 0x10000 + ((wc - 0xd800) << 10) + (wc2 - 0xdc00);
				return 2;
			}
		}
		else if (wc >= 0xdc00 && wc < 0xe000)
		{
			goto ilseq;
		}
		else
		{
			*pwc = wc;
			return 1;
		}
	}
	return RET_TOOFEW(0);

ilseq:
	return RET_SHIFT_ILSEQ(0);
}

int utf16_wctomb(uint16_t *r, uint32_t wc, uint32_t n)
{
	if (!(wc >= 0xd800 && wc < 0xe000))
	{
		if (wc < 0x10000)
		{
			if (n >= 1)
			{
				r[0] = (uint16_t)wc;
				return 1;
			}
			else
				return RET_TOOSMALL;
		}
		else if (wc < 0x110000)
		{
			if (n >= 2)
			{
				uint32_t wc1 = 0xd800 + ((wc - 0x10000) >> 10);
				uint32_t wc2 = 0xdc00 + ((wc - 0x10000) & 0x3ff);
				r[0] = (uint16_t)wc1;
				r[1] = (uint16_t)wc2;
				return 2;
			}
			else
				return RET_TOOSMALL;
		}
	}
	return RET_ILUNI;
}

//////////////////////////////////////////////////////////////////////////
uint32_t utf8to16(const char *strU8, uint32_t nU8, char16_t *strU16, uint32_t nU16, char16_t badChar)
{
	const uint8_t *mbs = (const uint8_t*)strU8;
	uint32_t remain = nU8;

	uint32_t tmpWC;
	uint16_t tmpU16[2];

	uint32_t itU16 = 0;

	while (remain)
	{
		int count = utf8_mbtowc(&tmpWC, mbs, remain);
		if (count > 0)
		{
			mbs += count;
			remain -= count;

			count = utf16_wctomb(tmpU16, tmpWC, 2);
			if (count > 0)
			{
				if (strU16)
				{
					if (itU16 + count <= nU16)
					{
						memcpy(strU16 + itU16, tmpU16, count * sizeof(uint16_t));
						itU16 += count;
					}
				}
				else
					itU16 += count;

				continue;
			}
		}
		else
		{
			mbs += 1;
			remain -= 1;
		}

		// 转码失败
		count = 1;
		if (strU16)
		{
			if (itU16 + count <= nU16)
			{
				strU16[itU16] = badChar;
				itU16 += count;
			}
		}
		else
			itU16 += count;
	}
	return itU16;
}

uint32_t utf16to8(const char16_t *strU16, uint32_t nU16, char *strU8, uint32_t nU8, char badChar)
{
	const uint16_t *mbs = (const uint16_t*)strU16;
	uint32_t remain = nU16;

	uint32_t tmpWC;
	uint8_t tmpU8[4];

	uint32_t itU8 = 0;

	while (remain)
	{
		int count = utf16_mbtowc(&tmpWC, mbs, remain);
		if (count > 0)
		{
			mbs += count;
			remain -= count;

			count = utf8_wctomb(tmpU8, tmpWC, 4);
			if (count > 0)
			{
				if (strU8)
				{
					if (itU8 + count <= nU8)
					{
						memcpy(strU8 + itU8, tmpU8, count * sizeof(uint8_t));
						itU8 += count;
					}
				}
				else
					itU8 += count;

				continue;
			}
		}
		else
		{
			mbs += 1;
			remain -= 1;
		}

		// 转码失败
		count = 1;
		if (strU8)
		{
			if (itU8 + count <= nU8)
			{
				strU8[itU8] = badChar;
				itU8 += count;
			}
		}
		else
			itU8 += count;
	}
	return itU8;
}
