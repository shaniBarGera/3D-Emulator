#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include "Camera.h"
#include <string>


using namespace std;

Scene::Scene() {
	activeCamera = 0;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeModel = -1;
	step = 10;

}

Scene::Scene(Renderer* renderer) {
	m_renderer = renderer;
	activeCamera = 0;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeModel = -1;
	step = 10;

}

Scene::~Scene() {
	for (int i = 0; i < cameras.size(); ++i) {
		delete[] cameras[i];
	}
}

void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName); 
	models.push_back(model);
	activeModel = models.size();
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	Camera* cam = cameras[activeCamera];
	for (size_t i = 0; i < models.size(); ++i) {
		 MeshModel* model = (MeshModel*)models[i]; //CHANGE
		 m_renderer->SetProjection(cam->projection);
		 m_renderer->SetCameraTransform(cam->cTransform);
		 m_renderer->SetObjectMatrices(model->_world_transform, model->_normal_transform);
		 m_renderer->DrawTriangles(&model->vertex_positions, &model->vertex_normal);
		 
		 
	}
	if (models.size() > 0) {
		m_renderer->SwapBuffers();
		m_renderer->ClearColorBuffer();
	}
	
	
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}


void Scene::showNormalsV() {

}

void Scene::showNormalsF() {

}

void Scene::bbox() {

}

void Scene::addPrim() {
	printf("ADD PRIM\n");
	MeshModel* model = new PrimMeshModel();
	models.push_back(model);

}
void Scene::addCam(string s) {
	Camera* cam = new Camera();
	cameras.push_back(cam);
}

void Scene::render() {

}

void Scene::rotate() {

}

void Scene::zoomIn() {

}

void Scene::zoomOut() {

}

void Scene::scale(int dx, int dy) {

}

void Scene::focus() {

}

void Scene::move() {

}

/*--------------------------------------------------------------------*/
/*                              CAMERA                                */
/*--------------------------------------------------------------------*/

mat4 Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	vec4 n = normalize(eye - at);
	vec4 u = normalize(cross(up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	return c * Translate(-eye);
}

Camera::Camera()
{
	projection = (1, 0, 0, 0,
				  0, 1, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 1); // orthogonal

	cTransform = (1, 0, 0, 0,
				  0, 1, 0, 0,
				  0, 0, 1, 0,
				  0, 0, 0, 1);
}

void Camera::setTransformation(const mat4& transform) {
	cTransform *= transform;
}

mat4 Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
	vec4 a = vec4(-(2 / (right - left)), 0.0, 0.0, 1.0);
	vec4 b = vec4(0.0, 2 / (top - bottom), 0.0, 1.0);
	vec4 c = vec4(0.0, 0.0, -2 / (zFar - zNear), 1.0);
	vec4 d = vec4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0);
	return mat4(a, b, c, d);
}


//mat4 Camera::Perspective(const float fovy, const float aspect,
//	const float zNear, const float zFar) {

//}

mat4 Camera::Frustum(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar) {
	vec4 a = vec4(2 * zNear / (right - left), 0.0, (right + left) / (right - left), 0.0);
	vec4 b = vec4(0.0, 2 * zNear / (top - bottom), (top + bottom) / (top - bottom), 0.0);
	vec4 c = vec4(0.0, 0.0, -(zFar + zNear) / (zFar - zNear), 0.0);
	vec4 d = vec4(0.0, 0.0, -1.0, 0.0);
	return mat4(a, b, c, d);
}