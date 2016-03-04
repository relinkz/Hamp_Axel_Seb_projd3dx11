#include "QuadTree.h"

QuadTree::QuadTree()
{
	this->quadStart = Vector3(0, 0, 0);
	this->quadEnd = Vector3(0, 0, 0);
	this->bottomLeft = nullptr;
	this->bottomRight = nullptr;
	this->topLeft = nullptr;
	this->topRight = nullptr;
	this->xMax = 0;
	this->xMin = 0;
	this->zMax = 0;
	this->zMin = 0;
}

QuadTree::QuadTree(Vector3 start, Vector3 end, int depth, int xMax, int xMin, int zMax, int zMin)
{
	this->quadStart = start;
	this->quadEnd = end;
	this->topLeft = nullptr;
	this->topRight = nullptr;
	this->bottomLeft = nullptr;
	this->bottomRight = nullptr;

	int width = end.x - start.x;
	int height = end.z - start.z;

	this->xMax = xMax;
	this->xMin = xMin;
	this->zMax = zMax;
	this->zMin = zMin;

	if (depth > 0)
	{

		this->bottomLeft = new QuadTree(start, Vector3((end.x - (width / 2)), 0, (end.z - (height / 2))), (depth - 1), xMax, xMin, zMax, zMin);

		this->bottomRight = new QuadTree(Vector3(start.x + (width / 2), 0, start.z), Vector3(end.x, 0, (end.z - (height / 2))), (depth - 1), xMax, xMin, zMax, zMin);

		this->topLeft = new QuadTree(Vector3(start.x, 0, start.z + (width / 2)), Vector3(end.x / 2, 0, end.z), (depth - 1), xMax, xMin, zMax, zMin);

		this->topRight = new QuadTree(Vector3(start.x + (width / 2), 0, start.z + (height / 2)), end, (depth - 1), xMax, xMin, zMax, zMin);

	}
}

QuadTree::~QuadTree()
{
	if (this->bottomLeft == nullptr)
	{

	}
	else
	{
		delete this->bottomLeft;
		delete this->bottomRight;
		delete this->topLeft;
		delete this->topRight;
	}
}

void QuadTree::setTreeData(vector<Object> &objects)
{
	int nrOfObjects = objects.size();
	for (int i = 0; i < nrOfObjects; i++)
	{
		giveObject(objects.at(i));
	}

}

vector<Object*> QuadTree::getObjectsToDraw(Vector3 position)
{
	if (position.x < this->xMin)
	{
		position.x = this->xMin;
	}
	if (position.x > this->xMax)
	{
		position.x = this->xMax;
	}
	if (position.z < this->zMin)
	{
		position.z = this->zMin;
	}
	if (position.z > this->zMax)
	{
		position.z = this->zMax;
	}
	if (this->isInsideQuad(position))
	{
		if (this->bottomLeft == nullptr)
		{
			return this->objects;
		}
		else
		{
			bool result = false;
			result = this->bottomLeft->isInsideQuad(position);
			if (this->bottomLeft->isInsideQuad(position))
			{
				return this->bottomLeft->getObjectsToDraw(position);
			}

			else if (this->bottomRight->isInsideQuad(position))
			{
				return this->bottomRight->getObjectsToDraw(position);
			}

			else if (this->topLeft->isInsideQuad(position))
			{
				return this->topLeft->getObjectsToDraw(position);
			}
			else
			{
				return this->topRight->getObjectsToDraw(position);
			}

		}

	}

}


//helper functions
bool QuadTree::giveObject(Object &object)
{
	bool result = false;
	
	float distanceToStart = lengthOf(this->quadStart, object.getPosition());
	float distanceToEnd = lengthOf(this->quadEnd, object.getPosition());

	float distance = lengthOf(this->quadEnd, this->quadStart);
	//if(distanceToStart < (this->quadEnd - this->quadStart).Length() && distanceToEnd < (this->quadEnd - this->quadStart).Length())
	//if (distanceToStart < distance && distanceToEnd < distance)
	if (this->isInsideQuad(object.getPosition()))
	{
		if(this->bottomLeft == nullptr)
		{
			addObject(object);
			result = true;
		}
		else
		{
			result = this->bottomLeft->giveObject(object);
			if (result == false)
			{
				result = this->bottomRight->giveObject(object);
			}
			if (result == false)
			{
				result = this->topLeft->giveObject(object);
			}
			if (result == false)
			{
				result = this->topRight->giveObject(object);
			}
		}

	}

	return result;
}

void QuadTree::addObject(Object &object)
{
	Object* objectPtr = &object;
	this->objects.push_back(objectPtr);
}

float QuadTree::lengthOf(Vector3 point1, Vector3 point2)
{
	point1 = Vector3(point1.x, 0, point1.z);
	point2 = Vector3(point2.x, 0, point2.z);
	return (point1 - point2).Length();
	//return (point1 - point2).LengthSquared();
}
bool QuadTree::isInsideQuad(Vector3 position)
{

	bool result = false;

	if (position.x >= this->quadStart.x && position.z >= this->quadStart.z)
	{
		if (position.x <= this->quadEnd.x && position.z <= this->quadEnd.z)
		{
			result = true;
		}
	}
	return result;
}

