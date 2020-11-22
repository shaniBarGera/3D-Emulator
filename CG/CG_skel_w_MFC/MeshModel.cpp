#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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

GLfloat get_max_z(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].z >= max) max = (*vertices)[i].z;
	}
	return max;
}

GLfloat get_min_z(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].z <= min) min = (*vertices)[i].z;
	}
	return min;
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



using namespace std;

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
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
	vector<vec3> normals;
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
			faces.push_back(issLine);
		}
		else if (lineType == "vn") {
			// vertex normal
			normals.push_back(vec3fFromStream(issLine));
			
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else if (lineType == "mtllib" || lineType == "usemtl" || lineType == "g") {

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
			vertex_normal.push_back(normals[it->vn[i] - 1]);

			
		}
	}

	min_x = get_min_x(&vertex_positions);
	min_y = get_min_y(&vertex_positions);
	min_z = get_min_z(&vertex_positions);
	max_z = get_max_z(&vertex_positions);
	max_x = get_max_x(&vertex_positions);
	max_y = get_max_y(&vertex_positions);


	vec4 a = vec4(1, 0, 0, -((max_x+min_x)/2));
	vec4 b = vec4(0, 1, 0, -((max_y+min_y)/2));
	vec4 c = vec4(0, 0, 1, -((max_z+min_z)/2));
	vec4 d = vec4(0, 0, 0, 1);
	m_transform = mat4(a,b,c,d);

	/*for (int i = 0; i < vertex_positions.size() - 1; i += 3)
	{
		vec3 v1 = vertex_positions[i];
		vec3 v2 = vertex_positions[i + 1];
		vec3 v3 = vertex_positions[i + 2];

		vec3 center = (v1 + v2 + v3) / 3;
		vertex_normal.push_back(center);
		vec3 normal = normalize(cross(v2 - v1, v3 - v1));
		vertex_normal.push_back(normal);
		//int f = i % 3;
		//faces[f]->v[0]
	}*/
}


void MeshModel::draw()
{
}
	