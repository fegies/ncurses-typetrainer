#pragma once
#include <string>
/*
	A "string" class using int types for storage
	Meant to store UTF-8 encoded inputs
*/

class Intstring
{
public:
	Intstring();
	Intstring(std::string& toencode);
	Intstring(int* chars,int length);
	~Intstring();

	Intstring& operator=(std::string& stdstring);

	//returns the char at the specified position
	int at(int pos);

	int length();

	bool empty();

	//returns a UTF-32 string containing the word before the specified position
	Intstring* getWordBefore(int pos);

private:

	//may reserve more space than needed
	void parseString(std::string& stdstring);

	int slength;
	int* contents;
};