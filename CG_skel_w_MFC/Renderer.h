#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include <cstring>
//#include "Scene.h"

class Light {
public:
	vec3 place = vec3(2,0,0);
	vec3 color = (0.5,0.5,0.5);
	GLfloat intensity = 0.5;
	bool active = true;
	vec3 dir = vec3(1,0,0);
	std::string type = "ambient";
	Light() = default;
};

using namespace std;
class Renderer
{
	float *m_outBuffer; // 3*width*height
	GLfloat *m_zbuffer; // width*height
	vector<vector<int>>* curr_poly;

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
	int m_width, m_height;
	vector<Light*> lights;

	bool show_normalsF = false;
	bool show_normalsV = false;
	bool uniform = true;

	vec3 pmin = vec3(-BIG_NUMBER, -BIG_NUMBER, -BIG_NUMBER);
	vec3 pmax = vec3(BIG_NUMBER, BIG_NUMBER, BIG_NUMBER);

	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec3>* eyes, const vector<vec3>* vertices,vec3 color, const vector<vec3>* normals = NULL, const vector<vec3>* vertex_bbox = NULL, vec4 fraction=1, vec3 pos_camera = NULL);
	void Drawline(vec3  p1, vec3 p2, vec3 color, bool save_poly = false);
	void SetCameraMatrices(const mat4& cTransform, const mat4& projection);
	void SetScreenTransform();
	void SetObjectMatrices(const mat4& mTranslate, const mat4& mTransform, const mat4& wTransform, const mat4& nTransform, const mat4& nwTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void reshape(int w, int h);
	bool setPixelOn(int x, int y, vec3 p1, vec3 p2, vec3 p3, vec3 color, 
		vec3 world_p1 = NULL, vec3  world_p2 = NULL, vec3  world_p3 = NULL,
		bool shade = false, vec3 eye = vec3(0, 0, 3), vec3 normal=vec3(0,0,0), vec4 fraction = vec4(1,1,1,1));
	void SetFlags(bool bbox, bool show_normalsV, bool show_normalsF, bool uniform);
	//void FillPolygon(vec3 p1, vec3 p2, vec3 p3, char color, vector<vector<int>>* curr_poly);
	void FillPolygon(vec3 color, vec3 p1, vec3 p2, vec3 p3, vec3 normal, vec4 fraction, vec3 eye,
		vec3 world_p1, vec3 world_p2, vec3 world_p3);
	void put_z(int x, int y, GLfloat Z);
	GLfloat get_z(int x, int y);
	GLfloat pointLight(Light* light, vec3 pixel, vec3 normal, vec4 fraction, vec3 eye, vec3 screen_pixel);
	GLfloat parallelLight(Light* light, vec4 fraction, vec3 eye, vec3 pixel, vec3 normal);
	GLfloat ambientLight(Light* l, vec4 fraction);
	void drawSkeleton(const vector<vec3>* vertices);
};
