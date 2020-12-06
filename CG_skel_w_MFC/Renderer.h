#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

using namespace std;
class Renderer
{
	float *m_outBuffer; // 3*width*height
	float *m_zbuffer; // width*height
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

	bool show_normalsF = false;
	bool show_normalsV = false;

	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec3>* eyes, const vector<vec3>* vertices,vec3 color, const vector<vec3>* normals = NULL, const vector<vec3>* vertex_bbox = NULL);
	void Drawline(vec3 p1, vec3 p2, vec3 color, bool save_poly = false);
	void SetCameraMatrices(const mat4& cTransform, const mat4& projection);
	void SetScreenTransform(GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y);
	void SetObjectMatrices(const mat4& mTranslate, const mat4& mTransform, const mat4& wTransform, const mat4& nTransform, const mat4& nwTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void reshape(int w,int h);
	bool setPixelOn(int x, int y, vec3 p1, vec3 p2, vec3 p3, vec3 color);
	void SetFlags(bool bbox, bool show_normalsV, bool show_normalsF);
	//void FillPolygon(vec3 p1, vec3 p2, vec3 p3, char color, vector<vector<int>>* curr_poly);
	void FillPolygon(vec3 color, vec3 p1, vec3 p2, vec3 p3);
	void put_z(int x, int y, GLfloat Z);
	GLfloat get_z(int x, int y);
};
