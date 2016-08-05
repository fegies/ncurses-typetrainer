#pragma once
#include <string>
/*
	A "string" class using int types for storage
	Meant to store UTF-8 encoded inputs in UTF-32 format
*/

class Intstring
{
public:
	Intstring();
	Intstring(std::string& toencode);
	Intstring(const char* cstr);
	Intstring(int* chars,int length);
	~Intstring();

	//deletes the internal buffers of the Intstring before copying
	Intstring& operator=(std::string& stdstring);
	Intstring& operator=(const char* cstr);

	//returns the char at the specified position,
	//returns -1 if attempted access is out of bounds
	int at(int pos);

	int length();
	bool empty();

	//returns a UTF-32 string containing the word before the specified position
	//used in feeding the tree
	Intstring* getWordBefore(int pos);

private:

	//may reserve more space than needed
	void parseString(std::string& stdstring);

	int slength;
	int* contents;
};