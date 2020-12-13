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
	GLfloat min = BIG_NUMBER;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].z <= min) min = (*vertices)[i].z;
	}
	return min;
}

GLfloat get_min_x(const vector<vec3>* vertices) {
	GLfloat min = BIG_NUMBER;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x <= min) min = (*vertices)[i].x;
	}
	return min;
}

GLfloat get_min_y(const vector<vec3>* vertices) {
	GLfloat min = BIG_NUMBER;
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

int MeshModel::found_v_index(vec3 v) {
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i] == v) return i;
	}
	return -1;
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
	
	// init v_normal
	vector<vector<vec3>> v_normal;
	for (int i = 0; i < vertices.size(); i++) {
		vec3 a = vec3(0, 0, 0);
		vector<vec3> b;
		b.push_back(a);
		//cout << b[0];
		v_normal.push_back(b);
	}
	// init v_normal_position
	for (int i = 0;i < 3*faces.size();i++) {
		vec3 a = vec3(0,0,0);
		vector<vec3> b;
		b.push_back(a);
		//cout << b[0];
		v_normal_position.push_back(b);
	}
	//cout << v_normal_position.size()<<"\n";
	// iterate through all stored faces and create triangles
	int k = 0;
	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
		{
			vertex_positions.push_back(vertices[it->v[i]-1]); //CHANGE
			vertex_normal.push_back(normals[it->vn[i] - 1]);
			v_normal[it->v[i] - 1].push_back(normals[it->vn[i] - 1]);
		}
	}
	//cout << v_normal.size()<<"\n";
	for (int j =0 ; j < vertices.size(); ++j) {
		int counter = 0;
		vec3 temp = vec3(0, 0, 0);
		int temp_size = v_normal[j].size();
		for (int i = 0;i < temp_size; i++) {
			counter++;
			temp += v_normal[j][i];
			
		}
		for (int i = 0;i < temp_size;i++) {
			v_normal[j].pop_back();
		}
		v_normal[j].push_back(temp / counter);
		//cout << v_normal[j][0];
	}
	for (int i = 0; i < vertex_positions.size();i++) {
		vec3 temp_normal = v_normal[found_v_index(vertex_positions[i])][0];
		//cout << temp_normal;
		v_normal_position[i].pop_back();
		v_normal_position[i].push_back(temp_normal);
	}
	//cout << v_normal.size() << "\n";
	//cout << vertices.size() << "\n";

	pmin.x = get_min_x(&vertex_positions);
	pmin.y = get_min_y(&vertex_positions);
	pmin.z = get_min_z(&vertex_positions);
	pmax.z = get_max_z(&vertex_positions);
	pmax.x = get_max_x(&vertex_positions);
	pmax.y = get_max_y(&vertex_positions);


	vec4 a = vec4(1, 0, 0, -((pmax.x+pmin.x)/2));
	vec4 b = vec4(0, 1, 0, -((pmax.y+pmin.y)/2));
	vec4 c = vec4(0, 0, 1, -((pmax.z+pmin.z)/2));
	vec4 d = vec4(0, 0, 0, 1);
	m_transform = mat4(a,b,c,d);

	// add bbox
	vec3 v000(pmin.x, pmin.y, pmin.z);
	vec3 v100(pmax.x, pmin.y, pmin.z);
	vec3 v110(pmax.x, pmax.y, pmin.z);
	vec3 v001(pmin.x, pmin.y, pmax.z);
	vec3 v010(pmin.x, pmax.y, pmin.z);
	vec3 v011(pmin.x, pmax.y, pmax.z);
	vec3 v101(pmax.x, pmin.y, pmax.z);
	vec3 v111(pmax.x, pmax.y, pmax.z);
	_add_line(v000, v100);
	_add_line(v000, v010);
	_add_line(v000, v001);
	_add_line(v100, v101);
	_add_line(v100, v110);
	_add_line(v010, v110);
	_add_line(v010, v011);
	_add_line(v110, v111);
	_add_line(v111, v101);
	_add_line(v001, v101);
	_add_line(v001, v011);
	_add_line(v011, v111);

}

void MeshModel::_add_line(vec3 v1, vec3 v2) {
	vertex_bbox.push_back(v1);
	vertex_bbox.push_back(v2);
}


void MeshModel::draw()
{
}
	