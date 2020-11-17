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
	
public:
	mat4 cTransform;
	mat4 projection;

	Camera();
	~Camera() {}
	void setTransformation(const mat4& transform);
	mat4 LookAt(const vec4& eye, const vec4& at, const vec4& up);
	mat4 Ortho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	mat4 Frustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	mat4 Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);
};

class Scene {

	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;

public:
	int activeModel;
	int activeLight;
	int activeCamera;
	int step;

	Scene();
	Scene(Renderer* renderer);
	~Scene();
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	
	void showNormalsV();
	void showNormalsF();
	void addPrim();
	void addCam(string s);
	void render();
	void rotate();
	void zoomIn();
	void zoomOut();
	void scale(int dx, int dy);
	void bbox();
	void focus();
	void move();
	
};