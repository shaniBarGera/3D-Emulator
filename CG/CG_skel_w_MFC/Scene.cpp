#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;


//-------------------------------------CTRS--------------------------------------------//
Scene::Scene() {
	activeCamera = 0;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeModel = -1;
	step_move = 0.005;
	step_rotate = 10;
	step_scale = 0.1;
}

Scene::Scene(Renderer* renderer) {
	m_renderer = renderer;
	activeCamera = 0;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	activeModel = -1;
	step_move = 0.005;
	step_rotate = 10;
	step_scale = 0.1;
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
		 m_renderer->SetScreenTransform(model->min_x, model->min_y, model->max_x, model->max_y);
		 m_renderer->SetCameraMatrices(cam->cTransform, cam->projection);
		 m_renderer->SetObjectMatrices(model->m_transform, model->_world_transform, model->_normal_transform);
		 m_renderer->SetFlags(model->bbox, model->show_normalsV, model->show_normalsF);
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

void Scene::_add_line(Model* m, vec3 v1, vec3 v2, vec3 v3) {
	MeshModel* model = (MeshModel*)m;
	model->vertex_positions.push_back(v1);
	model->vertex_positions.push_back(v2);
	model->vertex_positions.push_back(v3);
}

//-------------------------------------NORMALS--------------------------------------------//

void Scene::showNormalsV() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->show_normalsV = true;
}

void Scene::showNormalsF() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->show_normalsF = true;		
}

void Scene::removeNormalsV() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->show_normalsV = false;
}

void Scene::removeNormalsF() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->show_normalsF = false;
}

//-------------------------------------BBOX--------------------------------------------//


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

	GLfloat min_x = model->min_x;
	GLfloat min_y = model->min_y;
	GLfloat min_z = model->min_z;
	GLfloat max_x = model->max_x;
	GLfloat max_y = model->max_y;
	GLfloat max_z = model->max_z;

	vec3 v000(min_x, min_y, min_z);
	vec3 v100(max_x, min_y, min_z);
	vec3 v110(max_x, max_y, min_z);
	vec3 v001(min_x, min_y, max_z);
	vec3 v010(min_x, max_y, min_z);
	vec3 v011(min_x, max_y, max_z);
	vec3 v101(max_x, min_y, max_z);
	vec3 v111(max_x, max_y, max_z);
	_add_line(model, v000, v100, v100);
	_add_line(model, v000, v010, v010);
	_add_line(model, v000, v001, v001);
	_add_line(model, v100, v101, v101);
	_add_line(model, v100, v110, v110);
	_add_line(model, v010, v110, v110);
	_add_line(model, v010, v011, v011);
	_add_line(model, v111, v110, v111);
	_add_line(model, v111, v101, v101);
	_add_line(model, v001, v101, v101);
	_add_line(model, v001, v011, v011);
	_add_line(model, v011, v111, v111);
	model->bbox = true;
}

//-------------------------------------MOVE MODEL--------------------------------------------//

void Scene::rotate(char cord) {
	printf("ROTATE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	mat4 temp;
	GLfloat curr_step = step_rotate;
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
	model->_normal_transform = temp * model->_normal_transform;
}

void Scene::scale(char dir) {
	printf("SCALE %c\n", dir);
	MeshModel* model = (MeshModel*)models[activeModel];
	mat4 temp;
	mat4 temp_normal;
	GLfloat curr_step = step_scale;

	switch (dir) {
	case 'l':
		temp[0][0] -= curr_step;
		temp_normal[0][0] = 1 / temp[0][0];
		break;
	case 'r':
		temp[0][0] += curr_step;
		temp_normal[0][0] = 1 / temp[0][0];
		break;
	case 'u':
		temp[1][1] += curr_step;
		temp_normal[1][1] = 1 / temp[1][1];
		break;
	case 'd':
		temp[1][1] -= curr_step;
		temp_normal[1][1] = 1 / temp[1][1];
		break;
	case 'z':
		printf("z\n");
		print(temp);
		temp[2][2] += curr_step;
		print(temp);
		temp_normal[2][2] = 1 / temp[2][2];
		break;
	case 'Z':
		printf("Z\n");
		print(temp);
		temp[2][2] -= curr_step;
		print(temp);
		temp_normal[2][2] = 1 / temp[2][2];
		break;
	case '-':
		temp[0][0] -= curr_step;
		temp[1][1] -= curr_step;
		temp[2][2] -= curr_step;
		temp_normal[0][0] = 1 / temp[0][0];
		temp_normal[1][1] = 1 / temp[1][1];
		temp_normal[2][2] = 1 / temp[2][2];
		break;
	case '+':
		temp[0][0] += curr_step;
		temp[1][1] += curr_step;
		temp[2][2] += curr_step;
		temp_normal[0][0] = 1 / temp[0][0];
		temp_normal[1][1] = 1 / temp[1][1];
		temp_normal[2][2] = 1 / temp[2][2];
		break;
	}
	model->m_transform = temp * model->m_transform;
	model->_normal_transform = temp_normal * model->_normal_transform;

}

void Scene::zoomIn() {
	printf("zoomIn\n");
	focus();
	Camera* cam = (Camera*)cameras[activeCamera];
	mat4 temp;
	temp[0][0] += step_scale;
	temp[1][1] += step_scale;
	temp[2][2] += step_scale;
	cam->S *= temp;
	cam->cTransform *= cam->S;
}

void Scene::zoomOut() {
	printf("zoomOut\n");
	focus();
	Camera* cam = (Camera*)cameras[activeCamera];
	mat4 temp;
	temp[0][0] -= step_scale;
	temp[1][1] -= step_scale;
	temp[2][2] -= step_scale;
	cam->S *= temp;
	cam->cTransform *= cam->S;
}

void Scene::move(int dx, int dy) {
	printf("MOVE %d %d\n", dx, dy);

	MeshModel* model = (MeshModel*)models[activeModel];
	model->_world_transform[0][3] += step_move * dx;
	model->_world_transform[1][3] -= step_move * dy;
}

//-------------------------------------ADD TO SCENE--------------------------------------------//

void Scene::addPrim() {
	printf("ADD PRIM\n");
	MeshModel* model = new PrimMeshModel();
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::addCam(string cmd, vec3 eye, vec3 at, vec3 up) {
	Camera* cam = new Camera(eye, at, up);
	cameras.push_back(cam);
	activeCamera = cameras.size() - 1;
}


void Scene::render() {
	// Allow the user to choose if the cameras should be rendered (as a small plus sign for example)
}

void Scene::focus() {
	printf("FOCUS\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	Camera* cam = (Camera*)cameras[activeCamera];
	mat4 temp = model->_world_transform;
	cam->at = vec3(temp[0][3]/ temp[3][3], temp[1][3] / temp[3][3],temp[2][3] / temp[3][3]);
	print(cam->at);
	cam->LookAt(cam->eye, cam->at, cam->up);
}

void Scene::perspective(const float fovy, const float aspect, const float zNear, const float zFar) {
	Camera* cam = (Camera*)cameras[activeCamera];
	cam->Perspective(fovy, aspect, zNear, zFar);
}

void Scene::frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
	Camera* cam = (Camera*)cameras[activeCamera];
	cam->Frustum(left, right, bottom, top, zNear, zFar);
}

void Scene::ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
	Camera* cam = (Camera*)cameras[activeCamera];
	cam->Ortho(left, right, bottom, top, zNear, zFar);
}

/*--------------------------------------------------------------------*/
/*                              CAMERA                                */
/*--------------------------------------------------------------------*/



void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	printf("LOOK AT\n");

	/*printf("eye: ");
	print(eye);
	printf("at: ");
	print(at);
	printf("up: ");
	print(up);*/

	vec4 n = normalize(eye - at); // 0 0 1 0
	vec4 u = normalize(cross(up, n)); // 1 0 0  
	vec4 v = normalize(cross(n, u)); // 0 -1 0
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	cTransform =  c * Translate(-eye);
}

Camera::Camera()
{
	printf("CAMERA CTR\n");

	eye = vec3(0, 0, 3);
	at = vec3(0, 0, 0);
	up = vec3(0, 1, 0);
	LookAt(eye, at, up);
	Ortho(-2, 2, -2, 2, 0.5, -0.5);
	//Perspective(2, 2, 1, 2);
	//Frustum(-2, 2, -2, 2, 0.5, -0.5);
}

Camera::Camera(vec3 eye, vec3 at, vec3 up)
{
	printf("CAMERA CTR NEW\n");
	this->eye = eye;
	this->at = at;
	this->up = up;

	/*printf("eye: ");
	print(eye);
	printf("at: ");
	print(at);
	printf("up: ");
	print(up);*/

	LookAt(eye, at, up);
	//projection = Ortho();
}

void Camera::setTransformation(const mat4& transform) {
	//cTransform *= transform;
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
	vec4 a = vec4((2 / (right - left)), 0.0, 0.0, -(right + left) / (right - left));
	vec4 b = vec4(0.0, 2 / (top - bottom), 0.0, -(top + bottom) / (top - bottom));
	vec4 c = vec4(0.0, 0.0, -2 / (zFar - zNear), -(zFar + zNear) / (zFar - zNear));
	vec4 d = vec4(0.0, 0.0, 0.0, 1.0);
	projection = mat4(a, b, c, d);
}

void Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar) {
	vec4 a = vec4(1 / (aspect * tan(0.5f * fovy)), 0.0, 0.0, 0.0);
	vec4 b = vec4(0.0, 1 / tan(0.5f * fovy), 0.0, 0.0);
	vec4 c = vec4(0.0, 0.0, (-zNear - zFar) / (zNear - zFar), (2 * zNear * zFar) / (zNear - zFar));
	vec4 d = vec4(0.0, 0.0, -1.0, 0.0);
	//setTransformation(mat4(a, b, c, d));
	projection = mat4(a, b, c, d);
}


void Camera::Frustum(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar) {
	vec4 a = vec4(2 * zNear / (right - left), 0.0, (right + left) / (right - left), 0.0);
	vec4 b = vec4(0.0, 2 * zNear / (top - bottom), (top + bottom) / (top - bottom), 0.0);
	vec4 c = vec4(0.0, 0.0, -(zFar + zNear) / (zFar - zNear), (-2 * zNear * zFar)/(zFar - zNear));
	vec4 d = vec4(0.0, 0.0, -1.0, 0.0);
	projection = mat4(a, b, c, d);
}