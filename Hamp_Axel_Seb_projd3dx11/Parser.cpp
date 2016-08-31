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
			//getting the material library
			else if (data == "mtllib")
			{
				file >> this->mtllib;
			}
			//faces
			else if (data == "f")
			{
				triangleData face;
				for (int i = 0; i < 3; i++)
				{
					file >> data;
					face = this->stringToFace(data);
					this->loadDataIntoTriangleData(face);
				}
			}

		}
	}
	this->loadMaterial();
	file.close();
}

void Parser::forceSharedNormal()
{
	for (std::list<gFace>::iterator iterator = this->faceList.begin(), end = this->faceList.end(); iterator != end; ++iterator) 
	{

		iterator->a.nx = iterator->a.nx;
		iterator->a.ny = iterator->a.ny;
		iterator->a.nz = iterator->a.nz;

		iterator->b.nx = iterator->a.nx;
		iterator->b.ny = iterator->a.ny;
		iterator->b.nz = iterator->a.nz;

		iterator->c.nx = iterator->a.nx;
		iterator->c.ny = iterator->a.ny;
		iterator->c.nz = iterator->a.nz;
	}
}

void Parser::printFacesUVtoFile()
{
	ofstream f;
	f.open("uvOutPut.txt");
	int i = 0;

	TriangleVertex a;
	TriangleVertex b;
	TriangleVertex c;

	for (std::list<gFace>::iterator iterator = this->faceList.begin(), end = this->faceList.end(); iterator != end; ++iterator)
	{
		a = iterator->a;
		b = iterator->b;
		c = iterator->c;

		f << i++ << " " << a.u << " " << a.v << " | " << b.u << " " << b.v << " | " << c.u << " " << c.v << "\n";
	}
	f.close();
	
}

void Parser::loadMaterial()
{
	stringstream errorMessage;
	stringstream dataStream;
	string data = "";
	this->material = Material();

	ifstream file;
	file.open(this->mtllib);

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
triangleData Parser::stringToFace(const string& src)
{
	triangleData face;
	int s_size = src.size();
	int cutter = 0;
	string stringLeft = "";
	string source = src;

	if (src == "1/9/15")
	{
		true;
	}

	stringLeft = src;

	for (int i = 0; i < 3; i++)
	{
		//search string for first '/', save pos;
		cutter = source.find('/');

		//if '/' exists in string
		if (cutter != -1)
		{
			//cutstring into two
			stringLeft = source.substr(0, cutter);
			source.erase(0, cutter + 1);
		}
		else
		{
			stringLeft = source;
		}

		//convert string to int in face
		switch (i)
		{
		case 0:
			face.vIndex = stoi(stringLeft);
			break;
		case 1:
			face.txIndex = stoi(stringLeft);
			break;
		case 2:
			face.vNormIndex = stoi(stringLeft);
			break;
		}
	}

	return face;
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

void Parser::loadDataIntoTriangleData(triangleData &triangleDesc)
{
	//push front
	this->triVertex.push_back(triangleDesc);
}

void Parser::createList()
{
	//fatta vad som händer här
	TriangleVertex newVertex;
	triangleData temp;
	//Face temp
	int counter = 0;

	gFace currFace;
	int size = this->verticies.size();

	//Parsen
	while(this->triVertex.empty() != true)
	{
		temp = this->triVertex.front();

		newVertex.x = this->verticies.at(temp.vIndex-1).x;
		newVertex.y = this->verticies.at(temp.vIndex-1).y;
		newVertex.z = this->verticies.at(temp.vIndex-1).z;


		newVertex.u = this->UVtext.at(temp.txIndex-1).x;
		newVertex.v = this->UVtext.at(temp.txIndex-1).y;
		//open gl?
		newVertex.v = 1.0f - newVertex.v;

		newVertex.nx = this->vertexNormals.at(temp.vNormIndex - 1).x;
		newVertex.ny = this->vertexNormals.at(temp.vNormIndex - 1).y;
		//newVertex.nz = (-1) * this->vertexNormals.at(temp.vNormIndex - 1).z;
		newVertex.nz = this->vertexNormals.at(temp.vNormIndex - 1).z;

		this->finalVertexes.push_back(newVertex);
		this->triVertex.pop_front();

		counter %= 3;

		switch (counter++)
		{
		case 0:
			currFace.a = newVertex;
			break;
		case 1:
			currFace.b = newVertex;
			break;
		case 2:
			currFace.c = newVertex;
			this->faceList.push_front(currFace);
			break;
		}
	}
}

TriangleVertex Parser::popFirst()
{
	TriangleVertex temp;

	temp = this->finalVertexes.front();
	this->finalVertexes.pop_front();

	return temp;
}