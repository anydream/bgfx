#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
//#include <uchar.h>
#include <string.h>
#include <stdio.h>

//#include "Debug.h"

#define Assert(...)
#define AssertWarn(...)

//////////////////////////////////////////////////////////////////////////
// 检测编译器
#if defined(__clang__)
#  define COMPILER_CLANG	1
#elif defined(__GNUC__)
#  define COMPILER_GCC		1
#elif defined(_MSC_VER)
#  define COMPILER_MSVC		1
#else
#  error Not supported compiler!
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#  include <intrin.h>
#  define PLATFORM_IS_WINDOWS 1
#  if defined(_WIN64)
#    define PLATFORM_IS_WIN64 1
#  endif
#endif

#if defined(__ANDROID__)
#  define PLATFORM_IS_ANDROID 1
#endif

#if defined(__APPLE__)
#  define PLATFORM_IS_APPLE 1
#  include <TargetConditionals.h>
#  if TARGET_IPHONE_SIMULATOR
#    define PLATFORM_IS_IPHONE 1
#    define PLATFORM_IS_IPHONE_SIMULATOR 1
#  elif TARGET_OS_IPHONE
#    define PLATFORM_IS_IPHONE 1
#  elif TARGET_OS_MAC
#    define PLATFORM_IS_MAC 1
#  endif
#endif

#if defined(__linux__)
#  define PLATFORM_IS_LINUX 1
#endif

#if defined(__FreeBSD__)
#  define PLATFORM_IS_FREEBSD 1
#endif

#if defined(__unix__)
#  define PLATFORM_IS_UNIX 1
#endif

#if defined(__x86_64__) || defined(_M_X64)
#  define PLATFORM_ARCH_X86 1
#  define PLATFORM_IS_X64 1
#elif defined(__i386__) || defined(_M_IX86)
#  define PLATFORM_ARCH_X86 1
#  define PLATFORM_IS_X86 1
#endif

#if defined(__aarch64__)
#  define PLATFORM_ARCH_ARM 1
#  define PLATFORM_IS_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
#  define PLATFORM_ARCH_ARM 1
#  define PLATFORM_IS_ARM32 1
#endif

//////////////////////////////////////////////////////////////////////////
#if UINTPTR_MAX == UINT32_MAX
#  define PLATFORM_IS_32BIT 1
#else
#  define PLATFORM_IS_64BIT 1
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(__clang__) || defined(__GNUC__)
#  define PLATFORM_GNUC_LIKE 1
#  if defined(PLATFORM_ARCH_X86)
#    include <x86intrin.h>
#  elif defined(PLATFORM_ARCH_ARM)
#    include <arm_acle.h>
#    if defined(__ARM_NEON__)
#      include <arm_neon.h>
#    endif
#  endif
#elif defined(_MSC_VER)
#  define PLATFORM_MSVC_LIKE 1
#else
#  error Cannot detect compiler environment!
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(PLATFORM_ARCH_X86)
#  if defined(PLATFORM_GNUC_LIKE)
#    define PLATFORM_YIELD	__builtin_ia32_pause()
#  else
#    define PLATFORM_YIELD	_mm_pause()
#  endif
#elif defined(PLATFORM_ARCH_ARM)
#  define PLATFORM_YIELD	__yield()
#else
#  define PLATFORM_YIELD	std::this_thread::yield()
#endif

//////////////////////////////////////////////////////////////////////////
// 定义基本类型简写
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef intptr_t iptr;

typedef float f32;
typedef double f64;

//////////////////////////////////////////////////////////////////////////
// 定义 C/C++ 通用特性
#if defined(__cplusplus)
#  undef NULL
#  define NULL					nullptr
#  define CONSTEXPR				constexpr
#  define STATIC_ASSERT			static_assert
#  define ALIGNAS				alignas
#  define EXTERN_C				extern "C"
#else
#  define CONSTEXPR				const
#  define STATIC_ASSERT			_Static_assert
#  define ALIGNAS				_Alignas
#  define EXTERN_C
#endif

//////////////////////////////////////////////////////////////////////////
// 编译器属性检测器
#if defined(__has_attribute)
#  define HAS_ATTRIBUTE(x)		__has_attribute(x)
#else
#  define HAS_ATTRIBUTE(x)		0
#endif

#if defined(__has_builtin)
#  define HAS_BUILTIN(x)		__has_builtin(x)
#else
#  define HAS_BUILTIN(x)		0
#endif

//////////////////////////////////////////////////////////////////////////
// 尾递归
#if __JETBRAINS_IDE__
#  define TAIL_RETURN			return
#elif HAS_ATTRIBUTE(musttail)
#  define TAIL_RETURN			__attribute__((musttail)) return
#else
#  define TAIL_RETURN			return
#endif

// 弱链接
#if __JETBRAINS_IDE__
#  define WEAK					__declspec()
#elif HAS_ATTRIBUTE(weak)
#  define WEAK					__attribute__((weak))
#elif defined(COMPILER_MSVC)
#  define WEAK					__declspec(selectany)
#else
#  define WEAK
#endif

// 链接可见性
#if __JETBRAINS_IDE__
#  define LINK_VISIBLE				__declspec()
#elif HAS_ATTRIBUTE(visibility)
#  define LINK_VISIBLE				__attribute__((visibility("default")))
#elif defined(COMPILER_MSVC)
#  define LINK_VISIBLE				__declspec()
#else
#  define LINK_VISIBLE
#endif

// 禁止内联
#if __JETBRAINS_IDE__
#  define NOINLINE				__declspec()
#elif HAS_ATTRIBUTE(noinline)
#  define NOINLINE				__attribute__((noinline))
#elif defined(COMPILER_MSVC)
#  define NOINLINE				__declspec(noinline)
#else
#  define NOINLINE
#endif

// 强制内联
#if __JETBRAINS_IDE__
#  define ALWAYS_INLINE			__declspec()
#elif HAS_ATTRIBUTE(always_inline)
#  define ALWAYS_INLINE			__attribute__((always_inline))
#elif defined(COMPILER_MSVC)
#  define ALWAYS_INLINE			__forceinline
#else
#  define ALWAYS_INLINE
#endif

// 指定段位置
#if __JETBRAINS_IDE__
#  define SECTION(_name)		__declspec()
#  define SECTION_AT(_addr)		__declspec()
#elif HAS_ATTRIBUTE(section)
#  define SECTION(_name)		__attribute__((section(_name)))
#  if defined(__arm__)
#    define SECTION_AT(_addr)	__attribute__((section(".ARM.__at_" #_addr)))
#  endif
#else
#  define SECTION(_name)
#  define SECTION_AT(_addr)
//#  warning Section
#endif

//////////////////////////////////////////////////////////////////////////
// 分支优先级
#if HAS_BUILTIN(__builtin_expect)
#  define LIKELY(_cond)			__builtin_expect(!!(_cond), 1)
#  define UNLIKELY(_cond)		__builtin_expect(!!(_cond), 0)
#else
#  define LIKELY(_cond)			_cond
#  define UNLIKELY(_cond)		_cond
#endif

//////////////////////////////////////////////////////////////////////////
// 转换为字节指针
#define BytePtr(_expr)			((const uint8_t*)(_expr))
#define BytePtrMut(_expr)		((uint8_t*)(_expr))

//////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus)
// 引入 std::move/swap 等
#  include <utility>

// 模板特化条件
#  define EnableIf(_expr)		std::enable_if_t<_expr, int> = 0

// 类型可拷贝
#  define Def_Copyable(_cls)                            \
	constexpr _cls(const _cls &) noexcept = default;    \
	constexpr _cls& operator =(const _cls &) noexcept = default;

// 类型不可拷贝
#  define Def_NonCopyable(_cls)                         \
	_cls(const _cls &) = delete;                        \
	_cls& operator =(const _cls &) = delete;

// 类型可移动构造
#  define Def_MovableCtor(_cls)                         \
	_cls(_cls &&rhs) noexcept { swap(rhs); }

// 类型可移动赋值
#  define Def_MovableAssign(_cls)                       \
	_cls& operator =(_cls &&rhs) noexcept               \
	{                                                   \
		if (this != &rhs)                               \
		{                                               \
			_cls tmp = std::move(rhs);                  \
			swap(tmp);                                  \
		}                                               \
		return *this;                                   \
	}

// 类型可移动
#  define Def_Movable(_cls)                             \
	Def_MovableCtor(_cls)                               \
	Def_MovableAssign(_cls)

// 类型不可移动
#  define Def_NonMovable(_cls)                          \
	_cls(_cls &&) = delete;                             \
	_cls& operator =(_cls &&) = delete;

#  define Def_SwapBegin(_cls)                           \
	void swap(_cls &rhs) noexcept                       \
	{                                                   \
		if (this != &rhs)                               \
		{

#  define Def_Swap(_var)                                \
			std::swap(_var, rhs._var);

#  define Def_SwapEnd                                   \
		}                                               \
	}

// 获得元素个数
template <class T, size_t N>
[[nodiscard]]
constexpr size_t CountOf(T (&)[N])
{
	return N;
}

// 取大小
template <class T1, class T2>
[[nodiscard]]
constexpr auto MMin(T1 left, T2 right) noexcept(noexcept(left < right))
{
	return left < right ? left : right;
}

template <class T1, class T2>
[[nodiscard]]
constexpr auto MMax(T1 left, T2 right) noexcept(noexcept(left > right))
{
	return left > right ? left : right;
}

template <class T>
[[nodiscard]]
constexpr T& MMin(T &left, T &right) noexcept(noexcept(left < right))
{
	return left < right ? left : right;
}

template <class T>
[[nodiscard]]
constexpr T& MMax(T &left, T &right) noexcept(noexcept(left > right))
{
	return left > right ? left : right;
}

// 取绝对值
template <class T>
[[nodiscard]]
constexpr T MAbs(T n)
{
	if (n < 0)
		return -n;
	return n;
}

// 向上任意对齐
template <class T, class P>
[[nodiscard]]
constexpr T RoundUp(const T input, const P align)
{
	// 约束输入
	Assert(align > 0);

	T output;
	if (input > 0)
		output = (input + align - 1) / align * align;
	else
		output = input / align * align;

	// 警告输出
	AssertWarn(output >= input);
	return output;
}

// 向下任意对齐
template <class T, class P>
[[nodiscard]]
constexpr T RoundDown(const T input, const P align)
{
	// 约束输入
	Assert(align > 0);

	T output;
	if (input >= 0)
		output = input / align * align;
	else
		output = (input + align - 1) / align * align;

	// 警告输出
	AssertWarn(output <= input);
	return output;
}

// 向上对齐 2^N
template <class T>
[[nodiscard]]
constexpr T RoundUpPo2(const T input)
{
	// 约束输入
	Assert(input >= 0);

	T value = input;
	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	if constexpr (sizeof(T) >= 2)
		value |= value >> 8;
	if constexpr (sizeof(T) >= 4)
		value |= value >> 16;
	if constexpr (sizeof(T) >= 8)
		value |= value >> 32;
	const T output = value + 1;

	// 警告输出
	AssertWarn(output >= input);
	return output;
}

// 向下对齐 2^N
template <class T>
[[nodiscard]]
constexpr T RoundDownPo2(const T input)
{
	// 约束输入
	Assert(input >= 0);

	T value = input;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	if constexpr (sizeof(T) >= 2)
		value |= value >> 8;
	if constexpr (sizeof(T) >= 4)
		value |= value >> 16;
	if constexpr (sizeof(T) >= 8)
		value |= value >> 32;
	const T output = value - (value >> 1);

	// 警告输出
	AssertWarn(output <= input);
	return output;
}

// 判断是否为 2^N
template <class T>
[[nodiscard]]
constexpr bool IsPo2(T x)
{
	return (x & (x - 1)) == 0;
}

// 计算哈希值
template <class T>
[[nodiscard]]
uint32_t ArrayHash(const T *ary, uint32_t count, uint32_t hash = 0x14AE055C)
{
	for (uint32_t i = 0; i < count; ++i)
		hash = (hash + (hash << 5)) ^ ary[i];
	return hash;
}

template <class T, class P>
void ArrayFill(T *dst, const P &value, uint32_t count)
{
	while (count--)
		*dst++ = value;
}

template <class T>
void ArrayCopyNoOverlap(T *dst, const T *src, uint32_t count)
{
	// 无重叠情况
	while (count--)
		*dst++ = *src++;
}

template <class T>
void ArrayCopyOverlap(T *dst, const T *src, uint32_t count)
{
	if (dst <= src || dst >= src + count)
	{
		// 正向复制
		while (count--)
			*dst++ = *src++;
	}
	else
	{
		// 反向复制
		dst += count - 1;
		src += count - 1;

		while (count--)
			*dst-- = *src--;
	}
}

template <class T>
[[nodiscard]]
uint32_t ArrayCompare(const T *bufA, const T *bufB, uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		if (bufA[i] != bufB[i])
			return i + 1;
	}
	return 0;
}

// 大小写不敏感比较
template <class T>
[[nodiscard]]
uint32_t ArrayCompareCaseInsens(const T *bufA, const T *bufB, uint32_t count)
{
	constexpr auto toLower = +[](uint8_t ch)
	{
		return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch;
	};

	for (uint32_t i = 0; i < count; ++i)
	{
		if (toLower(bufA[i]) != toLower(bufB[i]))
			return i + 1;
	}
	return 0;
}

template <class T, class P>
[[nodiscard]]
uint32_t ArrayFillCompare(const T *buf, const P &value, uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		if (buf[i] != value)
			return i + 1;
	}
	return 0;
}

template <class T>
void StrCopyNoOverlap(T *dst, uint32_t dstCount, const T *src, uint32_t srcCount = -1)
{
	// 无重叠情况
	while (dstCount-- && srcCount--)
	{
		const T sch = *src++;
		*dst++ = sch;
		if (sch == 0)
			break;
	}
}

template <class T>
void StrCopyOverlap(T *dst, uint32_t dstCount, const T *src, uint32_t srcCount = -1)
{
	if (dst <= src || dst >= src + srcCount)
	{
		// 正向复制
		while (dstCount-- && srcCount--)
		{
			const T sch = *src++;
			*dst++ = sch;
			if (sch == 0)
				break;
		}
	}
	else
	{
		// 反向复制
		dst += srcCount - 1;
		src += srcCount - 1;

		while (dstCount-- && srcCount--)
		{
			const T sch = *src--;
			*dst-- = sch;
			if (sch == 0)
				break;
		}
	}
}

template <class T>
[[nodiscard]]
uint32_t StrCompare(const T *bufA, const T *bufB, uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		if (bufA[i] != bufB[i])
			return i + 1;

		// 遇到相同的 \0 返回成功
		if (bufA[i] == 0)
			return 0;
	}
	return 0;
}

// 大小写不敏感比较
template <class T>
[[nodiscard]]
uint32_t StrCompareCaseInsens(const T *bufA, const T *bufB, uint32_t count)
{
	constexpr auto toLower = +[](uint8_t ch)
	{
		return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch;
	};

	for (uint32_t i = 0; i < count; ++i)
	{
		if (toLower(bufA[i]) != toLower(bufB[i]))
			return i + 1;

		// 遇到相同的 \0 返回成功
		if (bufA[i] == 0)
			return 0;
	}
	return 0;
}

template <class T>
[[nodiscard]]
const volatile T& VolatileRead(const T &value)
{
	return static_cast<const volatile T&>(value);
}

template <class T>
[[nodiscard]]
volatile T& VolatileWrite(T &value)
{
	return static_cast<volatile T&>(value);
}

// 获得位掩码
template <class T>
[[nodiscard]]
constexpr T BitsMask(uint32_t lowBit, uint32_t highBit)
{
	constexpr uint32_t numBits = sizeof(T) * 8;
	constexpr T fullValue = T(-1);

	return (fullValue << lowBit) & (fullValue >> (numBits - 1 - highBit));
}

// 设置指定位数据
template <class T, class P>
[[nodiscard]]
constexpr T SetBits(T value, P subValue, uint32_t lowBit, uint32_t highBit)
{
	const T mask = BitsMask<T>(lowBit, highBit);
	value &= ~mask;
	value |= (T(subValue) << lowBit) & mask;
	return value;
}

// 获得指定位数据
template <class T>
[[nodiscard]]
constexpr T GetBits(T value, uint32_t lowBit, uint32_t highBit)
{
	const T mask = BitsMask<T>(lowBit, highBit);
	return (value & mask) >> lowBit;
}

// 从最低位开始找指定位
template <class T>
[[nodiscard]]
uint32_t FindBit(T value, bool bit = true)
{
#if HAS_BUILTIN(__builtin_ctz)
	if (sizeof(T) <= 4)
	{
		if (!bit)
			value = ~value;
		return MMin<uint32_t, uint32_t>(sizeof(T) * 8, __builtin_ctz(value));
	}
	else if (sizeof(T) <= 8)
	{
		if (!bit)
			value = ~value;
		return MMin<uint32_t, uint32_t>(sizeof(T) * 8, __builtin_ctzll(value));
	}

#endif

	const uint32_t allBits = sizeof(value) * 8;
	for (uint32_t i = 0; i < allBits; ++i)
	{
		if (!!((value >> i) & 1) == bit)
		{
			return i;
		}
	}
	return allBits;
}

// 拷贝 CVR 修饰符
template <class T, class U>
struct copy_cv_reference
{
private:
	using R = std::remove_reference_t<T>;
	using U1 = std::conditional_t<std::is_const_v<R>, std::add_const_t<U>, U>;
	using U2 = std::conditional_t<std::is_volatile_v<R>, std::add_volatile_t<U1>, U1>;
	using U3 = std::conditional_t<std::is_lvalue_reference_v<T>, std::add_lvalue_reference_t<U2>, U2>;
	using U4 = std::conditional_t<std::is_rvalue_reference_v<T>, std::add_rvalue_reference_t<U3>, U3>;

public:
	using type = U4;
};

template <class T, class U>
using copy_cv_reference_t = typename copy_cv_reference<T, U>::type;

// 从 this 自动转换成 [const] Derived*
#define CRTPThis	static_cast<copy_cv_reference_t<std::remove_pointer_t<decltype(this)>, Derived>*>(this)
// 从 const this 自动转换成 Derived*
#define CRTPThisMut	static_cast<Derived*>(const_cast<std::remove_const_t<std::remove_pointer_t<decltype(this)>>*>(this))

template <class P, class M>
[[nodiscard]]
constexpr size_t OffsetOf(const M P::*member)
{
	return (size_t)&((P*)(NULL)->*member);
}

template <class P, class M, class N>
[[nodiscard]]
constexpr P* ContainerOfImpl(const N *ptr, const M P::*member)
{
	static_assert(
		std::is_array_v<M>
			? std::is_convertible_v<std::decay_t<M>, N*>
			: std::is_convertible_v<M*, N*>);
	return (P*)((uint8_t*)ptr - OffsetOf(member));
}

#define ContainerOf(_ptr, _type, _member)	ContainerOfImpl(_ptr, &_type::_member)

#else
typedef uint16_t char16_t;

// 获得元素个数
#  define CountOf(_elem)			(sizeof(_elem) / sizeof((_elem)[0]))

// 取大小
#  define MMin(_left, _right)		((_left) < (_right) ? (_left) : (_right))
#  define MMax(_left, _right)		((_left) > (_right) ? (_left) : (_right))
// 取绝对值
#  define MAbs(_n)					((_n) < 0 ? -(_n) : (_n))
// 向上任意对齐
#  define RoundUp(_n, _align)		(((_n) > 0) ? (((_n) + (_align) - 1) / (_align) * (_align)) : ((_n) / (_align) * (_align)))
// 向下任意对齐
#  define RoundDown(_n, _align)		(((_n) >= 0) ? ((_n) / (_align) * (_align)) : (((_n) + (_align) - 1) / (_align) * (_align)))
// 判断是否为 2^N
#  define IsPo2(_n)					(((_n) & ((_n) - 1)) == 0)

// 向上对齐 2^N
inline uint32_t RoundUpPo2(const uint32_t input)
{
	uint32_t value = input;
	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	const uint32_t output = value + 1;

	// 警告输出
	AssertWarn(output >= input);
	return output;
}

inline uint64_t RoundUpPo2_64(const uint64_t input)
{
	uint64_t value = input;
	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value |= value >> 32;
	const uint64_t output = value + 1;

	// 警告输出
	AssertWarn(output >= input);
	return output;
}

// 向下对齐 2^N
inline uint32_t RoundDownPo2(const uint32_t input)
{
	uint32_t value = input;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	const uint32_t output = value - (value >> 1);

	// 警告输出
	AssertWarn(output <= input);
	return output;
}

inline uint64_t RoundDownPo2_64(const uint64_t input)
{
	uint64_t value = input;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value |= value >> 32;
	const uint64_t output = value - (value >> 1);

	// 警告输出
	AssertWarn(output <= input);
	return output;
}

#endif
