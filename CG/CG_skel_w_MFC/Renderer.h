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

	mat4 CTransform;
	mat4 MTransform;
	mat4 WTransform;
	mat4 Projection;
	mat4 NTransform;
	mat4 STransform;

	bool bbox;
	
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
	void DrawTriangles(const vector<vec3>* eyes, const vector<vec3>* vertices, const vector<vec3>* normals = NULL);
	void Drawline(int x1, int x2, int y1, int y2, char color);
	void SetCameraMatrices(const mat4& cTransform, const mat4& projection);
	void SetScreenTransform(GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y);
	void SetObjectMatrices(const mat4& mTransform, const mat4& wTransform, const mat4& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void reshape(int w,int h);
	void setPixelOn(int x, int y, char color);
	void SetFlags(bool bbox, bool show_normalsV, bool show_normalsF);
};
