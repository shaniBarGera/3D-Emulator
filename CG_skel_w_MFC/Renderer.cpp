#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <algorithm>

#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)

Renderer::Renderer() :m_width(512), m_height(512)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
}
Renderer::Renderer(int width, int height) :m_width(width), m_height(height)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
}

Renderer::~Renderer(void)
{
}



void Renderer::CreateBuffers(int width, int height)
{
	m_width=width;
	m_height=height;	
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3*m_width*m_height];
	m_zbuffer = new GLfloat[m_width * m_height];
	curr_poly = new vector<vector<int>>(m_width);
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,256,i,0)]=1;
		m_outBuffer[INDEX(m_width,256,i,1)]=0;	
		m_outBuffer[INDEX(m_width,256,i,2)]=0;

	}
	//horizontal line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,i,256,0)]=1;
		m_outBuffer[INDEX(m_width,i,256,1)]=0;
		m_outBuffer[INDEX(m_width,i,256,2)]=1;

	}
}

GLfloat Area(vec2 p1, vec2 p2, vec2 p3) {
	vec2 a = vec2(p1.x - p2.x, p1.y - p2.y);
	vec2 b = vec2(p3.x - p2.x, p3.y - p2.y);
	vec2 res = a * b;
	return sqrt(res.x * res.x + res.y * res.y);
}

GLfloat depth(int x, int y, vec3 p1, vec3 p2, vec3 p3) {
	//vec3 posiotion_x, vec3 posiotion_y, vec3 posiotion_z) {
	vec2 a = vec2(x, y);
	vec2 b = vec2(p1.x, p1.y);
	vec2 c = vec2(p2.x, p2.y);
	vec2 d = vec2(p3.x, p3.y);
	GLfloat a1 = Area(a, b, c);
	GLfloat a2 = Area(a, b, d);
	GLfloat a3 = Area(a, c, d);
	GLfloat sum_a = a1 + a2 + a3;
	return (a1 / sum_a) * p1.z + (a2 / sum_a) * p2.z + (a3 / sum_a) * p3.z;
}

bool Renderer::setPixelOn(int x, int y, vec3 p1, vec3 p2, vec3 p3, vec3 color) {
	//printf("SET PIXEL ON %d %d\n", x, y);
	if (x < 0 || x >= m_width || y < 0 || y >= m_width) { 
		return false; 
	}
	GLfloat z = depth(x, y, p1, p2, p3);
	if (z >= m_zbuffer[INDEXZ(m_width, x, y)]) {
		return false;
	}
	m_zbuffer[INDEXZ(m_width, x, y)] = z;
	
	m_outBuffer[INDEX(m_width - 1, x, y, 0)] = color.x;
	m_outBuffer[INDEX(m_width - 1, x, y, 1)] = color.y;
	m_outBuffer[INDEX(m_width - 1, x, y, 2)] = color.z;
	return true;
}

int Sign(int dxy)
{
	if (dxy < 0) return -1;
	else if (dxy > 0) return 1;
	else return 0;
}

void Renderer::Drawline(vec3 p1, vec3 p2, vec3 color, bool save_poly) {
	GLfloat x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
	//printf("draw line %d %d %d %d\n", x1, x2, y1, y2);
	int Dx = x2 - x1;
	int Dy = y2 - y1;

	//# Increments
	int Sx = Sign(Dx);
	int Sy = Sign(Dy);

	//# Segment length
	Dx = abs(Dx);
	Dy = abs(Dy);
	int D = max(Dx, Dy);

	//# Initial remainder
	double R = D / 2;

	int X = x1;
	int Y = y1;
	if (Dx > Dy)
	{
		//# Main loop
		for (int I = 0; I < D; I++)
		{
			setPixelOn(X, Y, p1, p2, p2, color);
			if (save_poly && X >= 0 && X < m_width) {
				(*curr_poly)[X].push_back(Y);
			}
	
			//# Update (X, Y) and R
			X += Sx; R += Dy; //# Lateral move
			if (R >= Dx)
			{
				Y += Sy;
				R -= Dx; //# Diagonal move
			}
		}
	}
	else
	{
		//# Main loop
		for (int I = 0; I < D; I++)
		{
			setPixelOn(X, Y, p1, p2, p2, color);
			if (save_poly && X >= 0 && X < m_width) {
				(*curr_poly)[X].push_back(Y);
			}

			//# Update (X, Y) and R
			Y += Sy;
			R += Dx; //# Lateral move
			if (R >= Dy)
			{
				X += Sx;
				R -= Dy; //# Diagonal move
			}
		}
	}
}


void Renderer::FillPolygon(vec3 color, vec3 p1, vec3 p2, vec3 p3){
	//vec3 posiotion_x, vec3 posiotion_y, vec3 posiotion_z){
	//printf("FILL POLY\n");
	Drawline(p1, p2, color, true);
	Drawline(p3, p2, color, true);
	Drawline(p1, p3, color, true);

	int min_x = max(min(min(p1.x, p2.x), p3.x), 0);
	int max_x = min(max(max(p1.x, p2.x), p3.x), m_height - 1);

	for (int x = min_x; x <= max_x; ++x) {
		if ((*curr_poly)[x].empty()){
			continue;
		}

		int max_y = min(*max_element((*curr_poly)[x].begin(), (*curr_poly)[x].end()), m_height-1);
		int min_y = max(*min_element((*curr_poly)[x].begin(), (*curr_poly)[x].end()), 0);

		for (int y = min_y; y <= max_y; ++y) {
			setPixelOn(x, y, p1, p2, p3, color);
		}
		(*curr_poly)[x].clear();
	}
}

void Renderer::DrawTriangles(const vector<vec3>* eye, const vector<vec3>* vertices, vec3 color,const vector<vec3>* normals, const vector<vec3>* vertices_bbox) {
	printf("DRAW TRIANGLE\n");
	
	// add cam renderer
	for (int j = 0; j < eye->size(); j++) {
		vec4 temp = vec4((*eye)[j]);
		vec3 new_temp = vec4t3(STransform * Projection * CTransform * temp);
		vec3 p1 = new_temp;
		vec3 p2 = new_temp;
		p1.x -= 3; p2.x += 3;
		Drawline(p1, p2, 'w');
		p1 = new_temp;
		p2 = new_temp;
		p1.y -= 3; p2.y += 3;
		Drawline(p1, p2, 'w');
	}

	// draw object
	for (int i = 0; i < vertices->size(); i+=3)
	{
		vec3 p[3];
		vec4 center(0);
		vec4 f_normal;
		
		for (int j = 0; j < 3; j++) {
			center += vec4((*vertices)[i + j]);
			vec4 temp = WTransform * MTransform * vec4((*vertices)[i + j]);
			vec4 normal = NWTransform * NTransform * vec4((*normals)[i + j]) * 0.2;
			normal.w = 0;
			
			if (j == 0) {
				f_normal = normal;
			}
			
			normal += temp;
			temp = STransform * Projection * CTransform * temp;

			normal = STransform * Projection * CTransform * normal;
			vec3 n = vec4t3(normal);

			p[j] = vec4t3(temp);

			if (show_normalsV) {
				Drawline(p[j], n, 'g');
			}
		}

		//curr_color = ((i / 3) % 2 == 0) ? 'r' : 'w';
		FillPolygon(color, p[0], p[1], p[2]);


		// draw normals
		vec4 v1 = (*vertices)[i];
		vec4 v2 = (*vertices)[i + 1];
		vec4 v3 = (*vertices)[i + 2];
		center = WTransform * MTransform * (center/=3);
		f_normal += center;

		center = STransform * Projection * CTransform * center;
		f_normal = STransform * Projection * CTransform * f_normal;
		vec3 n = vec4t3(f_normal);
		vec3 c = vec4t3(center);

		if (show_normalsF) {
			Drawline(c, n.x, 't');
		}
		
	}


	// draw bounding box
	if (!bbox) return;

	for (int i = 0; i < vertices_bbox->size(); i += 2)
	{
		printf("i:%d\n", i);
		vec3 p[2];

		for (int j = 0; j < 2; j++) {
			vec4 temp = WTransform * MTransform * vec4((*vertices_bbox)[i + j]);
			temp = STransform * Projection * CTransform * temp;
			vec3 v = vec4t3(temp);
			p[j] = v;

		}

		Drawline(p[0], p[1], 'r');
	}

}

void Renderer::ClearColorBuffer() {
	//clean bufer
	for (int i = 0; i < m_width; i++) {
		(*curr_poly)[i].clear();
		for (int j = 0; j < m_height; j++) {
			m_outBuffer[INDEX(m_width, i, j, 0)] = 0;
			m_outBuffer[INDEX(m_width, i, j, 1)] = 0;
			m_outBuffer[INDEX(m_width, i, j, 2)] = 0;
		}
	}
}

void Renderer::reshape(int width, int height) {
	CreateBuffers(width, height);
}

void Renderer::CreateLocalBuffer() {
}

/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[]={
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );

	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0 );

	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid *) sizeof(vtc) );
	glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );
	a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();

	glBindVertexArray(gScreenVtc);
	a = glGetError();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	a = glGetError();
	glutSwapBuffers();
	a = glGetError();
}

void Renderer::ClearDepthBuffer() {
	//clean bufer
	for (int i = 0; i < m_width; i++)
		for (int j = 0; j < m_height; j++) {
			m_zbuffer[INDEXZ(m_width, i, j)] = 10000;
		}
}

void Renderer::SetCameraMatrices(const mat4& cTransform, const mat4& projection) {
	CTransform = cTransform;
	Projection = projection;
}
void Renderer::SetScreenTransform(GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y) {
	STransform[0][3] = m_width / 2;
	STransform[1][3] = m_height / 2;
	STransform[0][0] = m_width / 2;
	STransform[1][1] = m_height / 2;
}
void Renderer::SetObjectMatrices(const mat4& mTranslate, const mat4& mTransform, const mat4& wTransform, const mat4& nTransform, const mat4& nwTransform) {
	MTransform = mTransform;
	NTransform = nwTransform * nTransform;
	WTransform = wTransform;
	MTranslate = mTranslate;

	
}

void Renderer::SetFlags(bool bbox, bool show_normalsV, bool show_normalsF) {
	this->bbox = bbox;
	this->show_normalsF = show_normalsF;
	this->show_normalsV = show_normalsV;
}

void Renderer::Init() {
	ClearColorBuffer();
	ClearDepthBuffer();
}