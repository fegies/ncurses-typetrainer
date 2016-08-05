#pragma once

#include <string>
#include <fstream>

/*
	The class responsible for reading the input text line by line,
	saving and restoring position in the textfile
	a.k.a. the typing progress
*/

class Textstream
{
public:
	Textstream();
	~Textstream();

	//lists the content of texts/ but withot .prog files or . and ..
	std::string* listfiles(int& count);

	void saveposition();

	void open_file(std::string filename);

	//returns an empty string if the end of the file was reached or no file is open
	std::string nextline();

	bool fileisopen();

	void loadposition();
private:
	//if this string is empty, no file is opened
	std::string openfile;

	//the beginning position of the line currently shown on the screen
	int lastposition;

	//strips everything not typable from the string
	void stripstring(std::string& string);

	std::ifstream in;
};
