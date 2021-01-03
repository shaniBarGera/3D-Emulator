#pragma once
#include <vector>
#include <cstring>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#define FILTER_SIZE 15
#define FILTER_HALF 7

class Light {
public:
	vec3 place = vec3(1,1,1);
	vec3 color = vec3(1,1,1);
	GLfloat intensity = 1;
	bool active = true;
	std::string type = "point";
	Light() = default;
};

using namespace std;
class Renderer
{
	float* m_outBuffer; // 3*width*height
	float* real_m_outBuffer;
	GLfloat* m_zbuffer; // width*height
	GLfloat* m_blurBuffer_x; // 3*width*height
	GLfloat* m_blurBuffer; // 3*width*height
	GLfloat* m_brightBuffer; // 3*width*height
	vector<vector<int>>* curr_poly;
	float threshold = 1;
	//GLfloat filter[FILTER_SIZE][FILTER_SIZE] = { 0 };
	GLfloat filter[FILTER_SIZE] = { 0 };

	mat4 CTransform;
	mat4 MTransform;
	mat4 WTransform;
	mat4 Projection;
	mat4 NTransform;
	mat4 STransform;
	mat4 MTranslate;
	mat4 NWTransform;

	bool bbox = false;


	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();
	

	//////////////////////////////
	// openGL stuff. Don't touch.

	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////
public:
	bool antialiasing = false;
	bool fogefect = true;
	bool blureffect = false;
	int m_width, m_height;
	int real_m_width = 0,real_m_height = 0;
	vector<Light*> lights;

	bool show_normalsF = false;
	bool show_normalsV = false;
	bool uniform = true;

	vec3 pmin = vec3(-BIG_NUMBER, -BIG_NUMBER, -BIG_NUMBER);
	vec3 pmax = vec3(BIG_NUMBER, BIG_NUMBER, BIG_NUMBER);
	char shade = 'f';

	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec3>* vertices, mat3 color,
		const vector<vec3>* normals , vec4 fraction, vec3 pos_camera, vector<vector<vec3>> avg_normals);
	void Drawline(vec3  p1, vec3 p2, vec3 color, bool save_poly = false);
	void SetCameraMatrices(const mat4& cTransform, const mat4& projection);
	void SetScreenTransform();
	void SetObjectMatrices(const mat4& mTranslate, const mat4& mTransform, const mat4& wTransform, const mat4& nTransform, const mat4& nwTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void reshape(int w, int h);
	void setBuffer();
	bool setPixelOn(vec3 pixel, vec3 color);
	void SetFlags(bool bbox, bool show_normalsV, bool show_normalsF, bool uniform);
	void FillPolygon(mat3 color, vec3 p1, vec3 p2, vec3 p3, vec3 normal1, vec3 normal2, vec3 normal3, vec3 normal_f, vec4 fraction, vec3 eye, mat3 world_ps);
	vec3 pointLight(Light* light, vec3 pixel, vec3 normal, vec4 fraction, vec3 eye, vec3 screen_pixel);
	vec3 parallelLight(Light* light, vec4 fraction, vec3 eye, vec3 pixel, vec3 normal);
	vec3 ambientLight(Light* l, vec4 fraction);
	void drawSkeleton(const vector<vec3>* vertices);
	void getGaussian();
	void applyFilter();
	void drawCameras(const vector<vec3>* eye);
	void blur();
	void anti_aliasing();
	void drawBBox(const vector<vec3>* vertices_bbox);
};