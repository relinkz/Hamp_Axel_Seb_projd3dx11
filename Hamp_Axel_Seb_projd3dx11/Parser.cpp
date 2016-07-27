#include "Parser.h"
#include <iostream>

using namespace std;

Parser::Parser()
{
	this->finished = false;
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

				this->verticies.push_back(Vector3(vertex[0], vertex[1], vertex[2]));
			}
			else if (data == "vt")
			{
				file >> UVcoord[0];
				file >> UVcoord[1];

				this->UVtext.push_back(Vector2(UVcoord[0], UVcoord[1]));
			}
			//handle the normals
			else if (data == "vn")
			{
				file >> vertexNormals[0];
				file >> vertexNormals[1];
				file >> vertexNormals[2];

				this->vertexNormals.push_back(Vector3(vertexNormals[0], vertexNormals[1], vertexNormals[2]));
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
				string* temp;
				for (int i = 0; i < 3; i++)
				{
					file >> data;
					this->loadDataIntoTriangleData(data);
				}
			}

		}
	}
	file.close();
}

void Parser::loadMaterial(const string& dest)
{
	stringstream errorMessage;
	stringstream dataStream;
	string data = "";
	this->material = Material();

	ifstream file;
	file.open(dest);

	if (file.is_open() == false)
	{
		errorMessage << "Could not find the destination" << endl;
	}
	else
	{
		while (file.eof() == false)
		{
			file >> data;

			if (data == "newmtl")
			{
				file >> data;
				this->material.setName(data);
			}
			else if (data == "illum")
			{
				file >> data;
				string::size_type size = data.size();
				this->material.setIlluminationModel(stoi(data, &size));
			}
			else if (data == "Kd")
			{
				string::size_type size = data.size();
				file >> data;
				Vector3 temp;
				temp.x = stof(data, &size);
				file >> data;
				temp.y = stof(data, &size);
				file >> data;
				temp.z = stof(data, &size);
				
				this->material.setDiffuse(temp);
			}
			else if (data == "Ka")
			{
				string::size_type size = data.size();
				file >> data;
				Vector3 temp;
				temp.x = stof(data, &size);
				file >> data;
				temp.y = stof(data, &size);
				file >> data;
				temp.z = stof(data, &size);

				this->material.setDiffuse(temp);
			}
			else if(data == "Tf")
			{
				string::size_type size = data.size();
				file >> data;
				Vector3 temp;
				temp.x = stof(data, &size);
				file >> data;
				temp.y = stof(data, &size);
				file >> data;
				temp.z = stof(data, &size);

				this->material.setTransmissionFilter(temp);
			}
			else if (data == "Ni")
			{
				file >> data;
				string::size_type size = data.size();
				this->material.setOpticalDensity(stof(data, &size));
			}
			else if (data == "Ks")
			{
				Vector3 temp;
				string::size_type size = data.size();
				file >> data;
				
				temp.x = stof(data, &size);
				file >> data;
				temp.y = stof(data, &size);
				file >> data;
				temp.z = stof(data, &size);

				this->material.setDiffuse(temp);
			}

			else if (data == "map_Kd")
			{
				file >> data;
				this->imageFile = data;
			}
			else
			{
				//throw("Cannot find argument in the register!");
			}
		}
	}
	file.close();

}
//getters
int Parser::getNrOfTriangleVertices() const
{
	return this->finalVertexes.size();
}

string Parser::getMtllib() const
{
	return this->mtllib;
}

string Parser::getGeometry() const
{
	return this->geometry;
}

string Parser::getUseMtl() const
{
	return this->usemtl;
}

string Parser::getImageFile() const
{
	return this->imageFile;
}

Vector3 Parser::getVertex(const int &nr) const
{
	return this->verticies[nr];
}

void Parser::loadDataIntoTriangleData(const string& triangleDesc)
{
	string strIndex[3] = { "","","" };
	stringstream test;

	int counter = 0;
	int nrOfCharacters = triangleDesc.size();
	string temp = "";
	int stringStart = 0;
	int vertexCount = 0;

	for (int i = 0; i < nrOfCharacters; i++)
	{
		if (triangleDesc[i] == '/')
		{
			//if / was hit
			for (int j=0; j < counter; j++)
			{
				//I am not a smart man, but it works... :)
				temp = triangleDesc.substr((i-counter), counter);
				strIndex[vertexCount] = temp;
				vertexCount++;
				counter = 0;
			}
		}
		else
		{
			//continue
			counter++;
			if (i == nrOfCharacters - 1)
			{
				//last part in the string
				temp = triangleDesc.substr((i - counter + 1), counter);
				strIndex[vertexCount] = temp;
			}
		}
	}


	triangleData data
	{
		0,0,0
	};
	string::size_type size = strIndex[0].size();
	data.vIndex = stoi(strIndex[0], &size);

	size = strIndex[1].size();
	data.txIndex = stoi(strIndex[1], &size);

	size = strIndex[2].size();
	data.vNormIndex = stoi(strIndex[2], &size);

	//push front
	this->triVertex.push_back(data);
}

void Parser::createList()
{
	TriangleVertex newVertex;
	triangleData temp;
	
	int size = this->verticies.size();

	while(this->triVertex.empty() != true)
	{
		temp = this->triVertex.front();

		newVertex.x = this->verticies.at(temp.vIndex-1).x;
		newVertex.y = this->verticies.at(temp.vIndex-1).y;
		newVertex.z = this->verticies.at(temp.vIndex-1).z;

		newVertex.u = this->UVtext.at(temp.txIndex-1).x;
		newVertex.v = this->UVtext.at(temp.txIndex-1).y;
		newVertex.v = 1.0f - newVertex.v;

		newVertex.nx = this->vertexNormals.at(temp.vNormIndex - 1).x;
		newVertex.ny = this->vertexNormals.at(temp.vNormIndex - 1).y;
		//newVertex.nz = (-1) * this->vertexNormals.at(temp.vNormIndex - 1).z;
		newVertex.nz = this->vertexNormals.at(temp.vNormIndex - 1).z;

		this->finalVertexes.push_back(newVertex);
		this->triVertex.pop_front();
	}
}

TriangleVertex Parser::popFirst()
{
	TriangleVertex temp;

	temp = this->finalVertexes.front();
	this->finalVertexes.pop_front();

	return temp;
}