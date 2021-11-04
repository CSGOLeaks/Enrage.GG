#pragma once

#include "auto.hpp"

namespace Horizon::Core
{

template<class T>
class Singleton
{
public:
	static T& Instance()
	{
		static T instance;
		return instance;
	}

	T* operator -> ()
	{
		return ( T* )( this );
	}

	const T* operator -> () const
	{
		return ( const T* )( this );
	}

protected:
	Singleton() = default;
	Singleton( const Singleton& ) = delete;
	Singleton& operator = ( const Singleton& ) = delete;
};

}