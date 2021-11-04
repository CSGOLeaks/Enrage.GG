#include "print.hpp"

namespace Horizon::Win32
{

const char* m_pPrintTypes[] =
{
	"[*]",
	"[warn]",
	"[err]",
};

const wchar_t* m_pWidePrintTypes[] =
{
	L"[*]",
	L"[warn]",
	L"[err]",
};

void DebugPrint( PrintType print_type, const char* message )
{
#ifdef DEBUG
	char output[4096] = { };
	
	auto type = m_pPrintTypes[static_cast<int>( print_type )];
	sprintf_s( output, "%s %s\r\n", type, message );

	printf( output );
#endif // DEBUG
}

void Trace( const char* format, ... )
{
#ifdef DEBUG
	char message[2048] = { };

	va_list args;
	va_start( args, format );
	int length = vsprintf_s( message, format, args );
	va_end( args );

	DebugPrint( PrintType::Trace, message );
#endif // DEBUG
}

void Warning( const char* format, ... )
{
#ifdef DEBUG
	char message[2048] = { };

	va_list args;
	va_start( args, format );
	int length = vsprintf_s( message, format, args );
	va_end( args );

	DebugPrint( PrintType::Warning, message );
#endif // DEBUG
}


void Error( const char* format, ... )
{
#ifdef DEBUG
	char message[2048] = { };

	va_list args;
	va_start( args, format );
	int length = vsprintf_s( message, format, args );
	va_end( args );

	DebugPrint( PrintType::Error, message );
#endif // DEBUG
}

}