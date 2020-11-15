﻿#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

GLfloat get_max_x(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (size_t i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x >= max) max = (*vertices)[i].x;
	}
	return max;
}

GLfloat get_max_y(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].y >= max) max = (*vertices)[i].y;
	}
	return max;
}

GLfloat get_min_x(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x <= min) min = (*vertices)[i].x;
	}
	return min;
}

GLfloat get_min_y(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].y <= min) min = (*vertices)[i].y;
	}
	return min;
}

struct FaceIdcs
{
	int v[4];
	int vn[4];
	int vt[4];

	FaceIdcs()
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream & aStream)
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for(int i = 0; i < 3; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}
};

vec3 vec3fFromStream(std::istream & aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream & aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel(string fileName)
{
	loadFile(fileName);
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	cout << fileName;
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
	vector<vec3> vertices;
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v") {
			vertices.push_back(vec3fFromStream(issLine));
		}
		else if (lineType == "f") {
			//cout << "YES";
			faces.push_back(issLine);
		}
		else if (lineType == "vn") {
			// vertex normal
			vertex_normal.push_back(vec3fFromStream(issLine));
			
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}
	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertex_positions should contain:
	//vertex_positions={v1,v2,v3,v1,v3,v4}

	// iterate through all stored faces and create triangles
	int k = 0;
	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
		{
			vertex_positions.push_back(vertices[it->v[i]-1]); //CHANGE
		}
	}
	if (bbox) {
		GLfloat max_x = get_max_x(&vertex_positions);
		GLfloat max_y = get_max_y(&vertex_positions);
		GLfloat min_x = get_min_x(&vertex_positions);
		GLfloat min_y = get_min_y(&vertex_positions);
		vec3 a = vec3(max_x, min_y, 1); vec3 b = vec3(max_x, max_y, 1); vec3 c = vec3(max_x, min_y, 1);
		vec3 d = vec3(max_x, max_y, 1); vec3 e = vec3(min_x, max_y, 1); vec3 f = vec3(max_x, max_y, 1);
		vec3 g = vec3(min_x, max_y, 1); vec3 h = vec3(min_x, min_y, 1); vec3 i = vec3(min_x, max_y, 1);
		vec3 j = vec3(min_x, min_y, 1); vec3 k = vec3(max_x, min_y, 1); vec3 l = vec3(min_x, min_y, 1);
		vertex_positions.push_back(a); vertex_positions.push_back(b); vertex_positions.push_back(c);
		vertex_positions.push_back(d); vertex_positions.push_back(e); vertex_positions.push_back(f);
		vertex_positions.push_back(g); vertex_positions.push_back(h); vertex_positions.push_back(i);
		vertex_positions.push_back(j); vertex_positions.push_back(k); vertex_positions.push_back(l);
	}
}



void MeshModel::draw()
{
	printf("Hello World");
}


void PrimMeshModel::add_cube() {
	MeshModel("cube.obj");
}

PrimMeshModel::PrimMeshModel(){
	add_cube();
}

