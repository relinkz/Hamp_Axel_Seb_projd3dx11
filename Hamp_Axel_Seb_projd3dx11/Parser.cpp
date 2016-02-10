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
	string data = "";

	float vertex[3];
	float UVcoord[2];
	float vertexNormals[3];
	int crap;

	ifstream file;
	file.open(dest);


	if (file.is_open() == false)
	{
		errorMessage << "Could not find the destination" << endl;
	}
	else
	{
		while(file.eof() == false)
		{
			file >> data;
			//handle the verticies
			if (data == "v")
			{
				file >> vertex[0];
				file >> vertex[1];
				file >> vertex[2];

				this->verticies.push_back(new Vector3(vertex[0], vertex[1], vertex[2]));
			}
			else if (data == "vt")
			{
				file >> UVcoord[0];
				file >> UVcoord[1];

				this->UVtext.push_back(new Vector2(UVcoord[0], UVcoord[1]));
			}
			//handle the normals
			else if (data == "vn")
			{
				file >> vertexNormals[0];
				file >> vertexNormals[1];
				file >> vertexNormals[2];

				this->vertexNormals.push_back(new Vector3(vertexNormals[0], vertexNormals[1], vertexNormals[2]));
			}
			//the geometry
			else if (data == "g")
			{
				//if needed
				file >> this->geometry;
			}
			//Type of OBJ description
			else if (data == "usemtl")
			{
				//if needed
				file >> this->usemtl;

			}

			else if (data == "mtllib")
			{
				file >> this->mtllib;
			}
			//faces
			else if (data == "f")
			{
				int posOfNextIndex;
				int** faceArray = new int*[3];
				string subString[3];
				string faceVertex[3] = { "", "", "" };

				for (int i = 0; i < 3; i++)
				{
					faceArray[i] = new int[3];
					file >> faceVertex[i];
				}


				//for every vertex
				//do it as a string an convert it into a int
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						posOfNextIndex = faceVertex[i].rfind('/');
						if (posOfNextIndex != -1)// 1/1/1 
							faceVertex[i].replace(posOfNextIndex, 1, "");
					}
				}
				intArr* toClass;
				for (int i = 0; i < 3; i++)
				{
					toClass = new intArr;
					toClass->a = (int)faceVertex[i].at(0) - 48;
					toClass->b = (int)faceVertex[i].at(1) - 48;
					toClass->c = (int)faceVertex[i].at(2) - 48;
					
					//faceArray[i][j] = (int)faceVertex[i].at(j) - 48; //ascii convert
					this->faces.push_back(toClass);
				}

			}

		}
	}
	file.close();
}