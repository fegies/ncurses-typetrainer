#include "felocale/Intstring.hpp"

Intstring::Intstring()
{
	slength = 0;
}

Intstring::~Intstring()
{
	if(slength > 0)
		delete[] contents;
}

Intstring::Intstring(std::string& stdstring)
{
	parseString(stdstring);
}

Intstring& Intstring::operator= (std::string& stdstring)
{
	if(slength > 0)
		delete[] contents;
	parseString(stdstring);
	return *this;
}

int Intstring::at(int pos)
{
	if(pos < 0 || pos >= slength)
		return -1;
	else
		return contents[pos];
}

int Intstring::length()
{
	return slength;
}

bool Intstring::empty()
{
	return slength == 0;
}

void Intstring::parseString(std::string& stdstring)
{
	slength = stdstring.length();
	contents = new int [slength];
	char cc;
	int c;
	int wp = 0;
	for(int i = 0; i < stdstring.length(); ++i)
	{
		cc = stdstring.at(i);

		//check wether the first bit is 1, e.g. it is a multibyte char
		if( (cc & 0x80) == 0x80)
		{
			c = 0;

			char bytecount = 0;
			char marker = 0x40;
			while ((cc & marker) == marker)
			{
				marker >>= 1;
				++bytecount;
			}

			//move the bits from the first byte over
			char cm = 1;
			char mask = 0;
			while(cm != marker)
			{
				mask |= cm;
				cm <<= 1; 
			}
			c |= (cc & mask);

			//move the remaining bytes in.
			//the length is cut appropiately
			for(int a = 0; a<bytecount; ++a)
			{
				c <<= 6;
				++i;
				c |= (stdstring.at(i) & 0x3f);
			}
			slength -= bytecount;
		}
		//standart ascii. No conversion necessary
		else
			c = cc;

		contents[wp++] = c;
	}
}