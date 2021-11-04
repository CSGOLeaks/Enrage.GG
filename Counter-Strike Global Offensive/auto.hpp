#pragma once

#include "os.hpp"

#include <string>
#include <memory>
#include <vector>

#include <map>

//#include "lua_engine.hpp"
//
//#define SOL_USING_CXX_LUA 1
//#define SOL_USING_CXX_LUAJIT 1
////#define SOL_SAFE_REFERENCES 1
//#define SOL_SAFE_FUNCTION_CALLS 1
//
////#include <sol/sol.hpp>
//#pragma comment(lib, "src/lua51.lib")

struct s_interface
{
	s_interface(const char* dll, const char* name, void* interfaceBase)
		: m_module(dll)
		, m_name(name)
		, m_interface(interfaceBase)
	{

	}

	const char* m_module;
	const char* m_name;
	void* m_interface;
};

//--------------------------------------------------------------------------------
//-- XorCompileTime.hpp
//
// Author: frk
// Date: 12.12.2015
//
//--------------------------------------------------------------------------------

#pragma once
#include <string>
#include <array>
#include <cstdarg>

#define BEGIN_NAMESPACE( x ) namespace x {
#define END_NAMESPACE }

BEGIN_NAMESPACE(strenc)

constexpr auto time = __TIME__;
constexpr auto seed = static_cast<int>(time[7]) + static_cast<int>(time[6]) * 10 + static_cast<int>(time[4]) * 60 + static_cast<int>(time[3]) * 600 + static_cast<int>(time[1]) * 3600 + static_cast<int>(time[0]) * 36000;

// 1988, Stephen Park and Keith Miller
// "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
// Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
// with 32-bit math and without division

template < int N >
struct RandomGenerator
{
private:
	static constexpr unsigned a = 16807; // 7^5
	static constexpr unsigned m = 2147483647; // 2^31 - 1

	static constexpr unsigned s = RandomGenerator< N - 1 >::value;
	static constexpr unsigned lo = a * (s & 0xFFFF); // Multiply lower 16 bits by 16807
	static constexpr unsigned hi = a * (s >> 16); // Multiply higher 16 bits by 16807
	static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16); // Combine lower 15 bits of hi with lo's upper bits
	static constexpr unsigned hi2 = hi >> 15; // Discard lower 15 bits of hi
	static constexpr unsigned lo3 = lo2 + hi;

public:
	static constexpr unsigned max = m;
	static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template <>
struct RandomGenerator< 0 >
{
	static constexpr unsigned value = seed;
};

template < int N, int M >
struct RandomInt
{
	static constexpr auto value = RandomGenerator< N + 1 >::value % M;
};

template < int N >
struct RandomChar
{
	static const char value = static_cast<char>(1 + RandomInt< N, 0x7F - 1 >::value);
};

template < size_t N, int K >
struct XorString
{
private:
	const char _key;
	std::array< char, N + 1 > _encrypted;
	bool decrypted = false;

	constexpr char enc(char c) const
	{
		return c ^ _key;
	}

	char dec(char c) const
	{
		return c ^ _key;
	}

public:
	template < size_t... Is >
	constexpr FORCEINLINE XorString(const char* str, std::index_sequence< Is... >) : _key(RandomChar< K >::value), _encrypted{ enc(str[Is])... }
	{
	}

	FORCEINLINE decltype(auto) decrypt(void)
	{
		if (!decrypted)
		{
			for (size_t i = 0; i < N; ++i)
			{
				_encrypted[i] = dec(_encrypted[i]);
			}
			_encrypted[N] = '\0';
			decrypted = true;
		}
		return _encrypted.data();
	}
};

//--------------------------------------------------------------------------------
//-- Note: XorStr will __NOT__ work directly with functions like printf.
//         To work with them you need a wrapper function that takes a const char*
//         as parameter and passes it to printf and alike.
//
//         The Microsoft Compiler/Linker is not working correctly with variadic 
//         templates!
//  
//         Use the functions below or use std::cout (and similar)!
//--------------------------------------------------------------------------------

static auto w_printf = [](const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf_s(fmt, args);
	va_end(args);
};

static auto w_printf_s = [](const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf_s(fmt, args);
	va_end(args);
};

static auto w_sprintf = [](char* buf, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
};

static auto w_sprintf_s = [](char* buf, size_t buf_size, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buf, buf_size, fmt, args);
	va_end(args);
};

//for compatibility with debug mode
struct debug_ret
{
private:
	const char* ret;

public:
	debug_ret(const char* str) : ret(str) { };

	auto decrypt()
	{
		return ret;
	}
};

#ifndef DEBUG
#define xor_raw( s ) ( strenc::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1>() ) )
#define sxor( s ) ( strenc::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1>() ).decrypt() )
#else
#define xor_raw( s ) ( [ ]{ strenc::debug_ret ret{ s }; return ret; }( ) )
#define sxor( s ) ( s )
#endif

END_NAMESPACE

#ifndef AUTH

#define get_object(object, NAME) (object)

#else

#define get_object(object, NAME) ((decltype(object))[]() -> DWORD { \
        const static int marker = 0xDEADBABE; \
        static DWORD _fiddler_##NAME = __LINE__ + 1; \
        static DWORD _keyiddler_##NAME = __LINE__; \
        static DWORD _offsetidller_##NAME = 0; \
        return (_fiddler_##NAME ^ _keyiddler_##NAME) - marker; \
    }())

#define get_object_no_cast(NAME) ([]() -> DWORD { \
        const static int marker = 0xDEADBABE; \
        static DWORD _fiddler_##NAME = __LINE__ + 1; \
        static DWORD _keyiddler_##NAME = __LINE__; \
        static DWORD _offsetidller_##NAME = 0; \
        return (_fiddler_##NAME ^ _keyiddler_##NAME) - marker; \
    }())

#endif

#include "vmpsdk.h"

#pragma once
#include <stdint.h>

//fnv1a 32 and 64 bit hash functions
// key is the data to hash, len is the size of the data (or how much of it to hash against)
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline const uint32_t hash_32_fnv1a(const void* key, const uint32_t len) {

	const char* data = (char*)key;
	uint32_t hash = 0x811c9dc5;
	uint32_t prime = 0x1000193;

	for (int i = 0; i < (int)len; ++i) {
		uint8_t value = data[i];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;

} //hash_32_fnv1a

inline const uint64_t hash_64_fnv1a(const void* key, const uint64_t len) {

	const char* data = (char*)key;
	uint64_t hash = 0xcbf29ce484222325;
	uint64_t prime = 0x100000001b3;

	for (int i = 0; i < len; ++i) {
		uint8_t value = data[i];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;

} //hash_64_fnv1a

inline constexpr uint32_t hash_32_fnv1a_const(const char* const str, const uint32_t value = val_32_const) noexcept {
	return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

inline constexpr uint64_t hash_64_fnv1a_const(const char* const str, const uint64_t value = val_64_const) noexcept {
	return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}


typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned char uchar;

template<u32 S, u32 A = 16807UL, u32 C = 0UL, u32 M = (1UL << 31) - 1>
struct LinearGenerator {
	static const u32 state = ((u64)S * A + C) % M;
	static const u32 value = state;
	typedef LinearGenerator<state> next;
	struct Split {
		typedef LinearGenerator< state, A* A, 0, M> Gen1;
		typedef LinearGenerator<next::state, A* A, 0, M> Gen2;
	};
};

template<u32 S, std::size_t index>
struct Generate {
	static const uchar value = Generate<LinearGenerator<S>::state, index - 1>::value;
};

template<u32 S>
struct Generate<S, 0> {
	static const uchar value = static_cast<uchar> (LinearGenerator<S>::value);
};

template<std::size_t...>
struct StList {};

template<typename TL, typename TR>
struct Concat;

template<std::size_t... SL, std::size_t... SR>
struct Concat<StList<SL...>, StList<SR...>> {
	typedef StList<SL..., SR...> type;
};

template<typename TL, typename TR>
using Concat_t = typename Concat<TL, TR>::type;

template<size_t s>
struct Count {
	typedef Concat_t<typename Count<s - 1>::type, StList<s - 1>> type;
};

template<>
struct Count<0> {
	typedef StList<> type;
};

template<size_t s>
using Count_t = typename Count<s>::type;

template<u32 seed, std::size_t index, std::size_t N>
constexpr uchar get_scrambled_char(const char(&a)[N]) {
	return static_cast<uchar>(a[index]) + Generate<seed, index>::value;
}

template<u32 seed, typename T>
struct cipher_helper;

template<u32 seed, std::size_t... SL>
struct cipher_helper<seed, StList<SL...>> {
	static constexpr std::array<uchar, sizeof...(SL)> get_array(const char(&a)[sizeof...(SL)]) {
		return{ { get_scrambled_char<seed, SL>(a)... } };
	}
};

template<u32 seed, std::size_t N>
constexpr std::array<uchar, N> get_cipher_text(const char(&a)[N]) {
	return cipher_helper<seed, Count_t<N>>::get_array(a);
}

template<u32 seed, typename T>
struct noise_helper;

template<u32 seed, std::size_t... SL>
struct noise_helper<seed, StList<SL...>> {
	static constexpr std::array<uchar, sizeof...(SL)> get_array() {
		return{ { Generate<seed, SL>::value ... } };
	}
};

template<u32 seed, std::size_t N>
constexpr std::array<uchar, N> get_key() {
	return noise_helper<seed, Count_t<N>>::get_array();
}


template<typename T>
struct array_info;

template <typename T, size_t N>
struct array_info<T[N]>
{
	typedef T type;
	enum { size = N };
};

template <typename T, size_t N>
struct array_info<const T(&)[N]> : array_info<T[N]> {};

template<u32 seed, std::size_t N>
class obfuscated_string {
private:
	std::array<uchar, N> cipher_text_;
	std::array<uchar, N> key_;
public:
	explicit constexpr obfuscated_string(const char(&a)[N]) : cipher_text_(get_cipher_text<seed, N>(a)), key_(get_key<seed, N>())
	{}

	operator std::string() const {
		char plain_text[N];
		for (volatile std::size_t i = 0; i < N; ++i) {
			volatile char temp = static_cast<char>(cipher_text_[i] - key_[i]);
			plain_text[i] = temp;
		}

		std::string temp{ plain_text, plain_text + N };
		for (volatile std::size_t i = 0; i < N; ++i)
			plain_text[i] = '\0';

		return temp;
	}
};

template<u32 seed, std::size_t N>
std::ostream& operator<< (std::ostream& s, const obfuscated_string<seed, N>& str) {
	s << static_cast<std::string>(str);
	return s;
}

#define RNG_SEED ((__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10  + \
              (__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600 + \
              (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000) + \
              (__LINE__ * 100000)


#define LIT(STR) \
    obfuscated_string<RNG_SEED, array_info<decltype(STR)>::size>{STR}