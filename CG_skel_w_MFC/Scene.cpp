 #include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;


//-------------------------------------CTRS--------------------------------------------//
Scene::Scene() {
	Camera* cam = new Camera();
	cameras.push_back(cam);
	Light* light = new Light();
	lights.push_back(light);
}

Scene::Scene(Renderer* renderer) {
	m_renderer = renderer;
	Camera* cam = new Camera();
	cameras.push_back(cam);
	Light* light = new Light();
	lights.push_back(light);
}

Scene::~Scene() {
	for (int i = 0; i < cameras.size(); ++i) {
		delete[] cameras[i];
	}
	for (int i = 0; i < lights.size(); ++i) {
		delete[] lights[i];
	}
}

void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName); 
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::test() {
	addPrim();
	/*scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');*/
	//move(100, 0, -100);
	//rotate('x');
	rotate('y');
	rotate('y');
	rotate('y');
	/*addPrim();
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	scale('-');
	rotate('x');
	rotate('y');
	rotate('y');
	rotate('y');
	move(-100, 0, 100);*/
	dimm();
	dimm();
	dimm();
	dimm();
	addLight();
	setLightType("point");
}

void Scene::draw()
{
	//printf("DRAW\n");
	m_renderer->ClearColorBuffer();
	vector<vec3> eyes;
	for (size_t i = 0; i < cameras.size(); ++i) {
		if (cameras[i]->rendered) 
			eyes.push_back(cameras[i]->eye);
	}
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	Camera* cam = cameras[activeCamera];
	m_renderer->lights = lights;
	m_renderer->drawCameras(&eyes);
	m_renderer->SetScreenTransform();
	m_renderer->SetCameraMatrices(cam->cTransform, cam->projection);
	for (size_t i = 0; i < models.size(); ++i) {
		 MeshModel* model = (MeshModel*)models[i];
		 //m_renderer->SetDemoBuffer();
		 m_renderer->SetObjectMatrices(model->m_translate, model->m_transform, model->_world_transform, model->_normal_transform, model->_normal_world_transform);
		 m_renderer->SetFlags(model->bbox, model->show_normalsV, model->show_normalsF, model->uniform);
		 m_renderer->DrawTriangles(&model->vertex_positions, model->color, &model->vertex_normal, model->fraction, cam->eye, model->v_normal_position);
		 m_renderer->drawBBox(&model->vertex_bbox);
	}
	m_renderer->blur();
	m_renderer->anti_aliasing();

	if (models.size() > 0) {
		m_renderer->SwapBuffers();
		m_renderer->ClearColorBuffer();
		m_renderer->ClearDepthBuffer();
	}
}

void Scene::uniform() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->uniform = !model->uniform;
}

void Scene::color(vec3 color, char type) {
	MeshModel* model = (MeshModel*)models[activeModel];
	switch (type) {
	case 'e':
		model->color[0] = color;
		break;
	case 'd':
		model->color[1] = color;
		break;
	case 's':
		model->color[2] = color;
		break;
	case 'a':
		model->color = mat3(color, color, color);
		break;
	}
}

void Scene::fog() {
	m_renderer->fogefect = !m_renderer->fogefect;
}

void Scene::blur() {
	m_renderer->blureffect = !m_renderer->blureffect;
}

void Scene::antialiasing() {
	m_renderer->antialiasing = !m_renderer->antialiasing;
	m_renderer->setBuffer();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}


//-------------------------------------NORMALS--------------------------------------------//

void Scene::showNormalsV() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->show_normalsV = !model->show_normalsV;
}

void Scene::showNormalsF() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->show_normalsF = !model->show_normalsF;
}

//-------------------------------------BBOX--------------------------------------------//


void Scene::bbox() {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->bbox = !model->bbox;
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

	if (model->frame == 'm') {
		model->m_transform = temp * model->m_transform;
		model->_normal_transform = temp * model->_normal_transform;
		
	} else if (model->frame == 'w') {
		model->_world_transform = temp * model->_world_transform;
		model->_normal_world_transform = temp * model->_normal_world_transform;
	}

}

void Scene::scale(char dir) {
	printf("SCALE %c %f\n", dir, step_scale);
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
	case 'n':
		temp[2][2] += curr_step;
		temp_normal[2][2] = 1 / temp[2][2];
		break;
	case 'f':
		temp[2][2] -= curr_step;
		temp_normal[2][2] = 1 / temp[2][2];
		break;
	case '-':
		temp[0][0] -= curr_step;
		temp[1][1] -= curr_step;
		temp[2][2] -= curr_step;
		temp_normal[0][0] -= curr_step;
		temp_normal[1][1] -= curr_step;
		temp_normal[2][2] -= curr_step;
		break;
	case '+':
		temp[0][0] += curr_step;
		temp[1][1] += curr_step;
		temp[2][2] += curr_step;
		temp_normal[0][0] += curr_step;
		temp_normal[1][1] += curr_step;
		temp_normal[2][2] += curr_step;
		break;
	}
	if (model->frame == 'm') {
		model->m_transform = temp * model->m_transform;
		model->_normal_transform = temp * model->_normal_transform;

	}
	else if (model->frame == 'w') {
		model->_world_transform = temp * model->_world_transform;
		model->_normal_world_transform = temp * model->_normal_world_transform;
	}
}

void Scene::zoomIn() {
	//printf("zoomIn\n");
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
	//printf("zoomOut\n");
	focus();
	Camera* cam = (Camera*)cameras[activeCamera];
	mat4 temp;
	temp[0][0] -= step_scale;
	temp[1][1] -= step_scale;
	temp[2][2] -= step_scale;
	cam->S *= temp;
	cam->cTransform *= cam->S;
}

void Scene::move(int dx, int dy, int dz) {
	//printf("MOVE %d %d\n", dx, dy);

	MeshModel* model = (MeshModel*)models[activeModel];
	model->_world_transform[0][3] += step_move * dx;
	model->_world_transform[1][3] -= step_move * dy;
	model->_world_transform[2][3] += step_move * dz;
}

//-------------------------------------ADD TO SCENE--------------------------------------------//

void Scene::addPrim() {
	//printf("ADD PRIM\n");
	MeshModel* model = new PrimMeshModel();
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::addCam(vec3 eye, vec3 at, vec3 up) {
	Camera* cam = new Camera(eye, at, up);
	cameras.push_back(cam);
	activeCamera = cameras.size() - 1;
}


void Scene::render() {
	// Allow the user to choose if the cameras should be rendered (as a small plus sign for example)
	Camera* cam = (Camera*)cameras[activeCamera];
	cam->rendered = !cam->rendered;
	
}

void Scene::camFrame(char frame) {
	Camera* cam = (Camera*)cameras[activeCamera];
	if (frame == 'w')
		cam->at = vec3(0, 0, 0);
	else if (frame == 'v')
		focus();
}

void Scene::camMove(char dir) {
	//printf("CAM MOVE\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	Camera* cam = (Camera*)cameras[activeCamera];
	mat4 temp = model->_world_transform;
	switch (dir) {
	case 'l':
		cam->eye.x -= step_cam;
		//printf("l:%f\n", cam->eye.x);
		break;
	case 'r':
		cam->eye.x += step_cam;
		//printf("r:%f\n", cam->eye.x);
		break;
	case 'u':
		cam->eye.y += step_cam;
		//printf("u:%f\n", cam->eye.y);
		break;
	case 'd':
		cam->eye.y -= step_cam;
		//printf("d:%f\n", cam->eye.y);
		break;
	case 'c':
		cam->eye.z -= step_cam;
		//printf("c:%f\n", cam->eye.z);
		break;
	case 'f':
		cam->eye.z += step_cam;
		//printf("f:%f\n", cam->eye.z);
		break;

	}

	cam->LookAt(cam->eye, cam->at, cam->up);
}

void Scene::focus() {
	//printf("FOCUS\n");
	MeshModel* model = (MeshModel*)models[activeModel];
	Camera* cam = (Camera*)cameras[activeCamera];
	mat4 temp = model->_world_transform;
	cam->at = vec3(temp[0][3]/ temp[3][3], temp[1][3] / temp[3][3],temp[2][3] / temp[3][3]);
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


void Scene::modelFrame(char frame) {
	MeshModel* model = (MeshModel*)models[activeModel];
	model->frame = frame;
}


/*--------------------------------------------------------------------*/
/*                              CAMERA                                */
/*--------------------------------------------------------------------*/



void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	//printf("LOOK AT\n");

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
	//printf("CAMERA CTR\n");

	eye = vec3(0, 0, 3);
	at = vec3(0, 0, 0);
	up = vec3(0, 1, 0);
	LookAt(eye, at, up);
	Ortho(-2, 2, -2, 2, 2, -2);
	//Perspective(2, 2, 1, 2);
	//Frustum(-2, 2, -2, 2, 0.5, -0.5);
}

Camera::Camera(vec3 eye, vec3 at, vec3 up)
{
	//printf("CAMERA CTR NEW\n");
	this->eye = eye;
	this->at = at;
	this->up = up;

	LookAt(eye, at, up);
	Ortho(-2, 2, -2, 2, 2, -2);
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

/*--------------------------------------------------------------------*/
/*                              LIGHT                                 */
/*--------------------------------------------------------------------*/

void Scene::shine(char dir) {
	MeshModel* model = (MeshModel*)models[activeModel];
	if(dir == '+') model->fraction.w += 1;
	else if (dir == '-' && model->fraction.w > 0) model->fraction.w -= 1;
	printf("SHINE: %f\n", model->fraction.w);
}

void Scene::diffuse(char dir) {
	MeshModel* model = (MeshModel*)models[activeModel];
	if (dir == '+' && model->fraction.y < 1) model->fraction.y += step_surface;
	else if (dir == '-' && model->fraction.y > 0) model->fraction.y -= step_surface;
}

void Scene::emissive(char dir) {
	MeshModel* model = (MeshModel*)models[activeModel];
	if (dir == '+' && model->fraction.x < 1) model->fraction.x += step_surface;
	else if (dir == '-' && model->fraction.x > 0) model->fraction.x -= step_surface;
}

void Scene::specular(char dir) {
	MeshModel* model = (MeshModel*)models[activeModel];
	if (dir == '+' && model->fraction.z < 1) model->fraction.z += step_surface;
	else if (dir == '-' && model->fraction.z > 0) model->fraction.z -= step_surface;
}

void Scene::addLight() {
	// add new light
	//printf("ADD LIGHT\n");
	Light* light = new Light();
	lights.push_back(light);
	activeLight = lights.size() - 1;

}

void Scene::deactivateLight() {
	// deactivate current light
	printf("DEACTIVATE LIGHT\n");
	Light* light = lights[activeLight];
	light->active = false;
}

void Scene::colorLight(vec3 color) {
	// color active light
	//printf("COLOR LIGHT\n");
	Light* light = lights[activeLight];
	light->color = color;

}

void Scene::positionLight(vec3 place) {
	// position active light
	printf("POSITION LIGHT\n");
	Light* light = lights[activeLight];
	light->place = place;
}

void Scene::orientLight(char cord) {
	// orient active light
	//printf("ORIENT LIGHT\n");
	Light* light = lights[activeLight];
	mat4 temp;
	GLfloat curr_step = step_rotate;
	switch(cord){
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
	light->place = vec4t3(temp * vec4(light->place));
}

void Scene::setLightType(string type) {
	// set light typeb (parallel/point/ambient)
	//printf("SET LIGHT TYPE\n");
	Light* light = lights[activeLight];
	light->type = type;
	if (type == "parallel") light->place = vec3(1, 0, 0);
}

void Scene::shade(char type) {
	printf("SHADE\n");
	m_renderer->shade = type;
}

void Scene::dimm() {
	printf("DIMM\n");
	Light* light = lights[activeLight];
	light->intensity -= 0.1;
}

void Scene::bright() {
	printf("BRIGHT\n");
	Light* light = lights[activeLight];
	light->intensity += 0.1;
}
