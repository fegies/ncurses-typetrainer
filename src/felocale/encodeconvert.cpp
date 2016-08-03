#include "felocale/encodeconvert.h"

namespace felocale
{

char to_utf8(int widechar,char* bytes)
{
	auto encodebytes = [&](char num){
		for(;num > 0; --num)
		{
			bytes[num] = 0x80;
			bytes[num] |= (widechar & 0x3f);
			widechar >>= 6;
		}
	};

	//ASCII Codes
	if( widechar < 0x80 )
	{
		bytes[0] = (char) widechar;
		return 1;
	}

	//two byte wide encodings
	if( widechar < 0x800 )
	{
		encodebytes(1);
		bytes[0] = 0xc0;
		bytes[0] |= (widechar & 0x1f);
		return 2;
	}

	//three byte wide encodings
	if( widechar < 0x10000 )
	{
		encodebytes(2);
		bytes[0] = 0xe0;
		bytes[0] |= (widechar & 0xf);
		return 3;
	}

	//four byte wide encodings
	encodebytes(3);
	bytes[0] = 0xf0;
	bytes[0] |= (widechar & 0x7);
	return 4;
}


std::string convertstring(Intstring& inputstring)
{
	std::string outstring;


	char a,bytes[4];

	for(int i=0;i < inputstring.length(); ++i)
	{
		a = to_utf8(inputstring.at(i),bytes);
		for(char b=0;b < a ; ++b)
		{
			outstring.push_back(bytes[b]);
		}
	}

	return outstring;
}

}