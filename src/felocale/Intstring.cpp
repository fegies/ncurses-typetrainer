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

Intstring::Intstring(const char* cstr)
{
	std::string stdstr = cstr;
	parseString(stdstr);
}

Intstring::Intstring(int* chars, int length)
{
	slength = length;
	contents = chars;
}

Intstring::Intstring(std::string& stdstring)
{
	parseString(stdstring);
}

Intstring& Intstring::operator= (std::string& stdstring)
{
	//parsestring allocates a new int*, so this must be done to avoid memory leaks
	if(slength > 0)
		delete[] contents;

	parseString(stdstring);
	return *this;
}

Intstring& Intstring::operator= (const char* cstr)
{
	std::string stdstr = cstr;
	return operator=(stdstr);
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

Intstring* Intstring::getWordBefore(int pos)
{
	if(pos < 0)
		return NULL;

	if(pos > slength)
		pos = slength;

	//find the beginning of the word
	int beg = pos-1;
	while (beg >= 0 && contents[beg]!=' ')
		--beg;
	++beg;
	
	//copy the found word over
	int len = pos - beg;
	int* con = new int[len];
	for(int i=0;beg < pos;++i,++beg)
		con[i] = contents[beg];

	return new Intstring(con, len);

}

void Intstring::parseString(std::string& stdstring)
{
	slength = stdstring.length();
	//A bit more space than needed is allocated, but that does not really matter
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