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
	void _add_line(vec3 v1, vec3 v2);
	GLfloat k = 1;
	
public:
	bool bbox = false;
	bool clip = false;
	bool show_normalsV = false;
	bool show_normalsF = false;
	bool uniform = true;
	char frame = 'm';
	vec3 color = vec3(0.5,0.5,0.5);
	vec3 pmin = vec3(-BIG_NUMBER, -BIG_NUMBER, -BIG_NUMBER);
	vec3 pmax = vec3(BIG_NUMBER, BIG_NUMBER, BIG_NUMBER);
	mat4 _world_transform;
	mat4 _normal_transform;
	mat4 _normal_world_transform;
	
	mat4 m_transform;
	mat4 m_translate;
	mat4 m_rotate;

	vec4 fraction = vec4(0.5,0.5,0,5); // surface coefficient. Ka, Kd, Ks, alpha



	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	vector<vec3> vertex_positions;
	vector<vec3> vertex_normal;
	vector<vec3> vertices;
	vector<vec3> vertex_bbox;

	vector<vector<vec3>> v_normal_position;
	vector<vec3> f_normal;
	int found_v_index(vec3 v);
	
};

class PrimMeshModel : public MeshModel
{
public:
	PrimMeshModel() : MeshModel("cube.obj"){}
};