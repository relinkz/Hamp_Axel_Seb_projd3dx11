#include "Parser.h"
#include <iostream>

using namespace std;

Parser::Parser()
{

}

Parser::~Parser()
{

}

void Parser::progressFile(const string& dest) throw(...)
{
	stringstream errorMessage;
	stringstream dataStream;
	dataStream << "C:/Users/Sebastian/Documents/3d-prog/Directx11 project/Hamp_Axel_Seb_projd3dx11/Hamp_Axel_Seb_projd3dx11" << dest;
	string data;

	ifstream file;
	file.open(dataStream.str());

	if (file.is_open() == false)
	{
		errorMessage << "Could not find the destination" << endl;
	}
	else
	{
		file >> data;
		cout << data << endl;
	}
	file.close();
}

string Parser::toString() const
{
	return "not implemented yet";
}