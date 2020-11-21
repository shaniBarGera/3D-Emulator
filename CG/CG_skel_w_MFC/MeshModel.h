#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}
	bool normal = false;
	
	
public:
	bool bbox = false;
	char frame = 'm';
	mat4 _world_transform;
	mat3 _normal_transform;
	mat4 m_transform;
	mat4 m_translate;
	mat4 transform;
	mat4 projection;

	GLfloat min_x;
	GLfloat min_y;
	GLfloat min_z;
	GLfloat max_z;
	GLfloat max_x;
	GLfloat max_y;

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	vector<vec3> vertex_positions;
	vector<vec3> vertex_normal;
	
};

class PrimMeshModel : public MeshModel
{
public:
	PrimMeshModel() : MeshModel("cube.obj"){}
};