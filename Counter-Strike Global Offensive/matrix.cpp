#include "matrix.hpp"

float* VMatrix::operator [] ( const std::uint32_t index )
{
	return m[index];
}
const float* VMatrix::operator [] ( const std::uint32_t index ) const
{
	return m[index];
}