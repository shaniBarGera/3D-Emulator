#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;
void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName); 
	models.push_back(model);
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	for (size_t i = 0; i < models.size(); ++i) {
		 MeshModel* model = (MeshModel*)models[i]; //CHANGE
		 model->draw();
		 m_renderer->SetProjection(model->projection);
		 m_renderer->SetObjectMatrices(model->_world_transform,model->_normal_transform);
		 m_renderer->DrawTriangles(&model->vertex_positions, &model->vertex_normal);
		 m_renderer->SwapBuffers();
		 m_renderer->ClearColorBuffer();
	}
	//m_renderer->ClearColorBuffer();
	//m_renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}

mat4 Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	vec4 n = normalize(eye - at);
	vec4 u = normalize(cross(up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	return c * Translate(-eye);
}

void Camera::setTransformation(const mat4& transform) {
	cTransform *= transform;
}

mat4 Camera::Ortho(const float left, const float right,	const float bottom, const float top,const float zNear, const float zFar) {
	vec4 a = vec4(-(2/(right - left)), 0.0, 0.0, 1.0);
	vec4 b = vec4(0.0,2/(top-bottom), 0.0, 1.0);
	vec4 c = vec4(0.0, 0.0, -2/(zFar - zNear), 1.0);
	vec4 d = vec4(-(right + left)/(right-left),-(top + bottom)/(top - bottom), -(zFar + zNear)/(zFar - zNear), 1.0);
	return mat4(a,b,c,d);
}


//mat4 Camera::Perspective(const float fovy, const float aspect,
//	const float zNear, const float zFar) {

//}

mat4 Camera::Frustum(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar) {
	vec4 a = vec4(2*zNear/(right - left), 0.0, (right + left) / (right - left), 0.0);
	vec4 b = vec4(0.0, 2 * zNear / (top - bottom), (top + bottom) / (top - bottom), 0.0);
	vec4 c = vec4(0.0, 0.0, -(zFar + zNear) / (zFar - zNear), 0.0);
	vec4 d = vec4(0.0,0.0,-1.0,0.0);
	return mat4(a, b, c, d);
}


void Scene::setNormals() {

}
void Scene::addPrim() {

}
void Scene::addCam() {
	Camera* cam = new Camera();
	cameras.push_back(cam);

}
void Scene::transform() {

}
void Scene::render() {

}