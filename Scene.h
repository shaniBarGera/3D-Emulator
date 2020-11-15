#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
using namespace std;

class Model {
protected:
	virtual ~Model() {}
	void virtual draw()=0;
};


class Light {

};

class Camera {
	mat4 cTransform;
	mat4 projection;

public:
	void setTransformation(const mat4& transform);
	mat4 LookAt(const vec4& eye, const vec4& at, const vec4& up );
	mat4 Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );
	mat4 Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );
	mat4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);
};

class Scene {

	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;

public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	
	void setNormals();
	void addPrim();
	void addCam();
	void transform();
	void render();

	int activeModel;
	int activeLight;
	int activeCamera;
};