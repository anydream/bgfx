#pragma once

#include "Encoding.h"
#include <string>
#include <string_view>

//////////////////////////////////////////////////////////////////////////
inline std::u16string utf8to16(const std::string_view &strU8, char16_t badChar = u'?')
{
	const uint32_t count = utf8to16(strU8.data(), uint32_t(strU8.size()), NULL, 0, badChar);

	std::u16string strU16;
	strU16.resize(count);
	utf8to16(strU8.data(), uint32_t(strU8.size()), strU16.data(), count, badChar);
	return strU16;
}

template <EnableIf(sizeof(wchar_t) == sizeof(char16_t))>
inline std::wstring utf8to16w(const std::string_view &strU8, wchar_t badChar = L'?')
{
	const uint32_t count = utf8to16(strU8.data(), uint32_t(strU8.size()), NULL, 0, badChar);

	std::wstring strU16;
	strU16.resize(count);
	utf8to16(strU8.data(), uint32_t(strU8.size()), (char16_t*)strU16.data(), count, badChar);
	return strU16;
}

inline std::string utf16to8(const std::u16string_view &strU16, char badChar = '?')
{
	const uint32_t count = utf16to8(strU16.data(), uint32_t(strU16.size()), NULL, 0, badChar);

	std::string strU8;
	strU8.resize(count);
	utf16to8(strU16.data(), uint32_t(strU16.size()), strU8.data(), count, badChar);
	return strU8;
}

template <EnableIf(sizeof(wchar_t) == sizeof(char16_t))>
std::string utf16to8(const std::wstring_view &strU16, char badChar = '?')
{
	const uint32_t count = utf16to8((const char16_t*)strU16.data(), uint32_t(strU16.size()), NULL,
	                                0, badChar);

	std::string strU8;
	strU8.resize(count);
	utf16to8((const char16_t*)strU16.data(), uint32_t(strU16.size()), strU8.data(), count, badChar);
	return strU8;
}
