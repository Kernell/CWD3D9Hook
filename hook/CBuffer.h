#include "stdafx.h"

class CBuffer : protected std::vector < char >
{
public:
	CBuffer( void )
	{
	}
	CBuffer( const void* pData, unsigned int uiSize )
	{
		AddBytes( pData, uiSize, 0 );
	}

	void Clear( void )
	{
		clear();
	}

	bool IsEmpty( void ) const
	{
		return empty();
	}

	void Reserve( unsigned int uiSize )
	{
		return reserve( uiSize );
	}

	// Comparison
	bool operator== ( const CBuffer& other ) const
	{
		return size() == other.size()
			&& std::equal( begin(), end(), other.begin() );
	}

	bool operator!= ( const CBuffer& other ) const
	{
		return !operator== ( other );
	}

	// Status
	void SetSize( unsigned int uiSize, bool bZeroPad = false )
	{
		unsigned int uiOldSize = (unsigned int)size();
		resize( uiSize );
		if( bZeroPad && uiSize > uiOldSize )
			memset( GetData() + uiOldSize, 0, uiSize - uiOldSize );
	}

	unsigned int GetSize( void ) const
	{
		return (unsigned int)size();
	}

	// Access
	char* GetData( unsigned int uiOffset = 0 )
	{
		return size() ? &at( uiOffset ) : NULL;
	}

	const char* GetData( unsigned int uiOffset = 0 ) const
	{
		return size() ? &at( uiOffset ) : NULL;
	}

	// Joining
	CBuffer operator+( const CBuffer& other ) const
	{
		CBuffer temp = *this;
		return temp += other;
	}

	CBuffer& operator+=( const CBuffer& other )
	{
		AddBytes( other.GetData(), other.GetSize(), GetSize() );
		return *this;
	}

	// Splitting
	CBuffer Mid( int iOffset, int iSize ) const
	{
		iOffset = Clamp < int >( 0, iOffset, GetSize() );
		iSize = Clamp < int >( 0, iSize, GetSize() - iOffset );
		return CBuffer( GetData() + iOffset, iSize );
	}

	CBuffer Head( unsigned int uiAmount ) const
	{
		return Mid( 0, uiAmount );
	}

	CBuffer Tail( unsigned int uiAmount ) const
	{
		uiAmount = min( uiAmount, GetSize() );
		return Mid( GetSize() - uiAmount, uiAmount );
	}

protected:
	void AddBytes( const void* pData, unsigned int uiLength, int iOffset, bool bToFromNetwork = false )
	{
		if( iOffset + uiLength > GetSize() )
			SetSize( iOffset + uiLength );
#ifdef CBUFFER_LITTLE_ENDIAN
		if ( bToFromNetwork )
		{
			char* pDest      = GetData () + iOffset + uiLength;
			const char* pSrc = (const char*)pData;
			while ( uiLength-- )
				*--pDest = *pSrc++;
		}
		else
#endif
			memcpy( GetData() + iOffset, pData, uiLength );
	}

	bool GetBytes( void* pData, unsigned int uiLength, int iOffset, bool bToFromNetwork = false ) const
	{
		if( iOffset + uiLength > GetSize() )
			return false;

#ifdef CBUFFER_LITTLE_ENDIAN
		if ( bToFromNetwork )
		{
			char* pDest      = (char*)pData;
			const char* pSrc = GetData () + iOffset + uiLength;
			while ( uiLength-- )
				*pDest++ = *--pSrc;
		}
		else
#endif
			memcpy( pData, GetData() + iOffset, uiLength );
		return true;
	}

	friend class CBufferWriteStream;
	friend class CBufferReadStream;
};