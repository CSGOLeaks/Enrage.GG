#pragma once

#include "auto.hpp"

namespace Horizon::Win32
{

enum class PrintType
{
	Trace = 0,
	Warning,
	Error,
};

void DebugPrint( PrintType print_type, const char* message );

void Trace( const char* format, ... );
void Warning( const char* format, ... );
void Error( const char* format, ... );
}