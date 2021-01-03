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

class Camera {
	
public:
	mat4 cTransform;
	mat4 projection;
	mat4 S;
	vec3 eye;
	vec3 at;
	vec3 up;
	bool rendered = false;

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
	Renderer *m_renderer;

public:
	vector<Light*> lights;
	vector<Model*> models;
	vector<Camera*> cameras;
	int activeModel = -1;
	int activeLight = 0;
	int activeCamera = 0;
	GLfloat step_move = 0.01;
	GLfloat step_scale = 0.1;
	GLfloat step_rotate = 10;
	GLfloat step_cam = 1;
	GLfloat step_surface = 0.1;
	
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
	void addPrim();
	void addCam(vec3 eye, vec3 at, vec3 up);
	void render();
	void rotate(char cord);
	void zoomIn();
	void zoomOut();
	void bbox();
	void focus();
	void scale(char dir);
	void move(int dx, int dy, int dz = 0);
	void modelFrame(char frame);
	void camMove(char dir);
	void camFrame(char frame);
	void uniform();
	void color(vec3 color, char type = 'a');
	void test();

	void clip(vec3 p1, vec3 p2);
	void addLight();
	void deactivateLight();
	void colorLight(vec3 color);
	void positionLight(vec3 place);
	void orientLight(char cord);
	void setLightType(string type);
	void shade(char type);
	void dimm();
	void bright();
	void blur();
	void shine(char dir);
	void diffuse(char dir);
	void emissive(char dir);
	void specular(char dir);
	void antialiasing();
	void fog();
};