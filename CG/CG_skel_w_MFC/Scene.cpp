#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;

//-------------------------------------HELPERS--------------------------------------------//

GLfloat max_of_x(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (size_t i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x >= max) max = (*vertices)[i].x;
	}
	return max;
}

GLfloat max_of_y(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].y >= max) max = (*vertices)[i].y;
	}
	return max;
}

GLfloat max_of_z(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (size_t i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].z >= max) max = (*vertices)[i].z;
	}
	return max;
}

GLfloat min_of_z(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].z <= min) min = (*vertices)[i].z;
	}

	return min;
}

GLfloat min_of_x(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x <= min) min = (*vertices)[i].x;
	}

	return min;
}

GLfloat min_of_y(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].y <= min) min = (*vertices)[i].y;
	}
	return min;
}


//-------------------------------------CTRS--------------------------------------------//
Scene::Scene() {
	activeCamera = 0;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeModel = -1;
	step = 0.01;
	printf("EMPTY SCENE\n");

}

Scene::Scene(Renderer* renderer) {
	m_renderer = renderer;
	activeCamera = 0;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeModel = -1;
	step = 0.01;
	printf("SCENE %d %d\n", m_renderer->m_width, m_renderer->m_height);

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
	activeModel = models.size() - 1;
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	Camera* cam = cameras[activeCamera];
	for (size_t i = 0; i < models.size(); ++i) {
		 MeshModel* model = (MeshModel*)models[i];
		 m_renderer->SetProjection(cam->projection);
		 m_renderer->SetCameraTransform(cam->cTransform);
		 m_renderer->SetObjectMatrices(model->m_transform, model->_world_transform, model->_normal_transform);
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

//-------------------------------------NORMALS--------------------------------------------//

void Scene::showNormalsV() {

}

void Scene::showNormalsF() {

}

//-------------------------------------BB0X--------------------------------------------//

void Scene::_add_line(Model* m, vec3 v1, vec3 v2, vec3 v3) {
	MeshModel* model = (MeshModel*)m;
	model->vertex_positions.push_back(v1);
	model->vertex_positions.push_back(v2);
	model->vertex_positions.push_back(v3);
}

void Scene::unbbox() {
	printf("RESIZE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	if (!model->bbox) return;
	model->vertex_positions.resize(model->vertex_positions.size() - 18);
	model->bbox = false;
}

void Scene::bbox() {
	printf("BBOX\n");

	MeshModel* model = (MeshModel*)models[activeModel];
	if (model->bbox) return;

	GLfloat min_x = min_of_x(&model->vertex_positions);
	GLfloat min_y = min_of_y(&model->vertex_positions);
	GLfloat min_z = min_of_z(&model->vertex_positions);
	GLfloat max_x = max_of_x(&model->vertex_positions);
	GLfloat max_y = max_of_y(&model->vertex_positions);
	GLfloat max_z = max_of_x(&model->vertex_positions);

	vec3 v000(min_x, min_y, min_z);
	vec3 v100(max_x, min_y, min_z);
	vec3 v110(max_x, max_y, min_z);
	vec3 v001(min_x, min_y, max_z);
	vec3 v010(min_x, max_y, min_z);
	vec3 v011(min_x, max_y, max_z);
	vec3 v101(max_x, min_y, max_z);
	vec3 v111(max_x, max_y, max_z);
	_add_line(model, v000, v100, v110);
	_add_line(model, v000, v010, v011);
	_add_line(model, v000, v001, v101);
	_add_line(model, v100, v101, v111);
	_add_line(model, v001, v011, v111);
	_add_line(model, v010, v110, v111);

	model->bbox = true;
}

//-------------------------------------MOVE MODEL--------------------------------------------//

void Scene::rotate(char cord) {
	printf("ROTATE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	mat4 temp;
	GLfloat curr_step = 360 * step;
	switch (cord) {
	case 'x':
		temp = RotateX(curr_step);
		break;
	case 'y':
		temp = RotateY(curr_step);
		break;
	case 'z':
		temp = RotateZ(curr_step);
		break;
	case 'X':
		temp = RotateX(-curr_step);
		break;
	case 'Y':
		temp = RotateY(-curr_step);
		break;
	case 'Z':
		temp = RotateZ(-curr_step);
		break;
	}
	model->m_transform = temp * model->m_transform;
}

void Scene::zoomIn() { // TODO
	mat4 temp = Scale(vec3(step, step, step));
	temp[3][3] = 0.0;
	m_renderer->S += temp;
}

void Scene::zoomOut() { // TODO
	mat4 temp = Scale(vec3(step, step, step));
	temp[3][3] = 0.0;
	m_renderer->S -= temp;
}

void Scene::scale(char dir) {
	printf("SCALE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	mat4 temp;
	GLfloat curr_step = step;

	switch (dir) {
	case 'l':
		temp[0][0] -= curr_step;
		break;
	case 'r':
		temp[0][0] += curr_step;
		break;
	case 'u':
		temp[1][1] += curr_step;
		break;
	case 'd':
		temp[1][1] -= curr_step;
		break;
	}
	print(temp);
	model->m_transform *= temp;
}

void Scene::move(int dx, int dy) {
	printf("MOVE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	print(model->m_transform);
	model->m_transform[0][3] += step * dx;
	model->m_transform[1][3] -= step * dy;
	print(model->m_transform);
}

void Scene::movex(char dir) {
	printf("MOVE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	mat4 temp(0.0);
	GLfloat curr_step = step;

	switch (dir) {
	case 'l':
		temp[0][3] = -curr_step;
		break;
	case 'r':
		temp[0][3] = curr_step;
		break;
	case 'u':
		temp[1][3] = curr_step;
		break;
	case 'd':
		temp[1][3] = -curr_step;
		break;
	}
	model->m_transform += temp;
}

void Scene::wframe() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->_world_transform = Translate((m_renderer->m_width / 2) + 1, (m_renderer->m_height / 2) + 1, 0.0);
}
void Scene::mframe() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->_world_transform = mat4();
}
//-------------------------------------ADD TO SCENE--------------------------------------------//

void Scene::addPrim() {
	printf("ADD PRIM\n");
	MeshModel* model = new PrimMeshModel();
	models.push_back(model);
	activeModel = models.size() - 1;

}
void Scene::addCam(string s) {
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeCamera = cameras.size() - 1;
}


void Scene::render() {

}

void Scene::focus() {
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