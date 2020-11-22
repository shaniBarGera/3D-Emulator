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
	mat4 S;
	vec3 eye;
	vec3 at;
	vec3 up;

	Camera();
	Camera(vec3 eye, vec3 at, vec3 up);
	~Camera() {}
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Frustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);
};

class Scene {

	
	vector<Light*> lights;
	
	Renderer *m_renderer;

	void _add_line(Model* model, vec3 v1, vec3 v2, vec3 v3);

public:
	vector<Model*> models;
	vector<Camera*> cameras;
	int activeModel;
	int activeLight;
	int activeCamera;
	GLfloat step_move;
	GLfloat step_scale;
	GLfloat step_rotate;

	
	Scene();
	Scene(Renderer* renderer);
	~Scene();
	void loadOBJModel(string fileName);
	void perspective(const float fovy, const float aspect, const float zNear, const float zFar);
	void frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
	void ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);

	void draw();
	void drawDemo();
	
	void showNormalsV();
	void showNormalsF();
	void removeNormalsV();
	void removeNormalsF();
	void addPrim();
	void addCam(string s, vec3 eye, vec3 at, vec3 up);
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