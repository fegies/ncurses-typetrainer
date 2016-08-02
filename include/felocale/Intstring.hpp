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
	~Intstring();

	Intstring& operator=(std::string& stdstring);

	//returns the char at the specified position
	int at(int pos);

	int length();

	bool empty();

private:

	//may reserve more space than needed
	void parseString(std::string& stdstring);

	int slength;
	int* contents;
};