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
	Camera(vec3 v);
	~Camera() {}
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho(const float left, const float right,
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

	void _add_line(Model* model, vec3 v1, vec3 v2, vec3 v3);

public:
	int activeModel;
	int activeLight;
	int activeCamera;
	GLfloat step;
	
	Scene();
	Scene(Renderer* renderer);
	~Scene();
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	
	void showNormalsV();
	void showNormalsF();
	void addPrim();
	void addCam(string s, vec3 v);
	void render();
	void rotate(char cord);
	void zoomIn();
	void zoomOut();
	void bbox();
	void unbbox();
	void focus();
	void scale(char dir);
	void move(int dx, int dy);
	
};