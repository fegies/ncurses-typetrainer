#pragma once

#include <string>
#include <fstream>

class Textstream
{
public:
	Textstream();
	~Textstream();

	std::string* listfiles(int& count);

	void saveposition();

	void open_file(std::string filename);

	//returns an empty string if the end of the file was reached or no file is open
	std::string nextline();

	bool fileisopen();

private:
	//if this string is empty, no file is opened
	std::string openfile;

	void loadposition();

	//strips everything not typable from the string
	void stripstring(std::string& string);

	std::ifstream in;
};
