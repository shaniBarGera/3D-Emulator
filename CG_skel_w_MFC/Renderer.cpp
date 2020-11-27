#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :m_width(512), m_height(512)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
	bbox = false;
}
Renderer::Renderer(int width, int height) :m_width(width), m_height(height)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
	bbox = false;
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

bool Renderer::_setPixelOn(int x, int y, int r, int g, int b) {
	if (m_outBuffer[INDEX(m_width - 1, x, y, 0)] != r ||
		m_outBuffer[INDEX(m_width - 1, x, y, 1)] != g ||
		m_outBuffer[INDEX(m_width - 1, x, y, 2)] != b) {
		m_outBuffer[INDEX(m_width - 1, x, y, 0)] = r;
		m_outBuffer[INDEX(m_width - 1, x, y, 1)] = g;
		m_outBuffer[INDEX(m_width - 1, x, y, 2)] = b;
		return true;
	}
	return false;

}

bool Renderer::setPixelOn(int x, int y, char color) {
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return true;
	
	switch (color) {
	case 'r' :
		return _setPixelOn(x, y, 1, 0, 0);
		break;
	case 'g':
		return _setPixelOn(x, y, 0, 1, 0);
		break;
	case 'b':
		return _setPixelOn(x, y, 0, 0, 1);
		break;
	case 'p':
		return _setPixelOn(x, y, 1, 0, 1);
		break;
	case 't':
		return _setPixelOn(x, y, 0, 1, 1);
		break;
	case 'w':
		return _setPixelOn(x, y, 1, 1, 1);
		break;
	}

	return false;
}

int Sign(int dxy)
{
	if (dxy < 0) return -1;
	else if (dxy > 0) return 1;
	else return 0;
}

void Renderer::Drawline(int x1, int x2, int y1, int y2, char color) {
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
			setPixelOn(X, Y, color);
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
			setPixelOn(X, Y, color);
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


void Renderer::FillPolygon(vec3 p1, vec3 p2, vec3 p3, char color){
	int min_x = min(min(p1.x, p2.x), p3.x) + 1;
	int max_x = max(max(p1.x, p2.x), p3.x);
	int min_y = min(min(p1.y, p2.y), p3.y) + 1;
	int max_y = max(max(p1.y, p2.y), p3.y);
	for (int i = min_x; i < max_x; ++i) {
		bool first_x = true;
		for (int j = min_y; j < max_y; ++j) {
			if (!setPixelOn(i, j, color)) { // if the pixel was already on
				if (first_x)
					first_x = false;
				else
					break;
			}
		}
	}
}


void Renderer::DrawTriangles(const vector<vec3>* eye, const vector<vec3>* vertices, const vector<vec3>* normals) {
	//printf("DRAW TRIANGLE\n");

	for (int j = 0; j < eye->size(); j++) {
		vec4 temp = vec4((*eye)[j]);
		temp = STransform * Projection * CTransform * temp;
		GLfloat x = temp.x / temp.w;
		GLfloat y = temp.y /temp.w;
		Drawline(x - 3, x + 3, y, y, 'w');
		Drawline(x, x, y - 3, y + 3, 'w');
	}

	int length = (bbox) ? vertices->size() - 36 : vertices->size();
	for (int i = 0; i < length; i+=3)
	{
		GLfloat x[3] = { 0 };
		GLfloat y[3] = { 0 };
		vec4 center(0);

		vec4 f_normal;
		
		for (int j = 0; j < 3; j++) {
			center += vec4((*vertices)[i + j]);
			vec4 temp = WTransform * MTransform * vec4((*vertices)[i + j]);
			vec4 normal = NTransform * vec4((*normals)[i + j]) * 0.2;
			normal.w = 0;
			
			if (j == 0) {
				f_normal = normal;
			}

			
			normal += temp;
			temp = STransform * Projection * CTransform * temp;

			normal = STransform * Projection * CTransform * normal;
			vec3 n = vec4t3(normal);

			vec3 v = vec4t3(temp);
			x[j] = v.x;
			y[j] = v.y;


			if (show_normalsV) {
				Drawline(x[j], n.x, y[j], n.y, 'g');
			}
		}
		
		char color = 'p';

		Drawline(x[0], x[1], y[0], y[1], color);
		Drawline(x[2], x[1], y[2], y[1], color);
		Drawline(x[0], x[2], y[0], y[2], color);
	
		FillPolygon(vec3(x[0], y[0], 0), vec3(x[1], y[1], 0),
			vec3(x[2], y[2], 0), 't');


		vec4 v1 = (*vertices)[i];
		vec4 v2 = (*vertices)[i + 1];
		vec4 v3 = (*vertices)[i + 2];
		center = (v1 +v2 +v3) / 3;
		vec4 temp_center = WTransform * MTransform * center;
		f_normal += temp_center;

		temp_center = STransform * Projection * CTransform * temp_center;
		f_normal = STransform * Projection * CTransform * f_normal;
		vec3 n = vec4t3(f_normal);
		vec3 c = vec4t3(temp_center);

		if (show_normalsF) {
			Drawline(c.x, n.x, c.y, n.y, 't');
		}
		
	}

	for (int i = length; i < vertices->size(); i += 3)
	{
		GLfloat x[3] = { 0 };
		GLfloat y[3] = { 0 };

		for (int j = 0; j < 3; j++) {
			vec4 temp = WTransform * MTransform * vec4((*vertices)[i + j]);
			temp = STransform * Projection * CTransform * temp;
			vec3 v = vec4t3(temp);
			x[j] = v.x;
			y[j] = v.y;

		}

		char color = 'r';

		Drawline(x[0], x[1], y[0], y[1], color);
		Drawline(x[2], x[1], y[2], y[1], color);
		Drawline(x[0], x[2], y[0], y[2], color);
	}

}

void Renderer::ClearColorBuffer() {
	//clean bufer
	for (int i = 0; i < m_width; i++) {
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
			m_zbuffer[INDEX(m_width, i, j, 0)] = 0;
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
void Renderer::SetObjectMatrices(const mat4& mTranslate, const mat4& mTransform, const mat4& wTransform, const mat4& nTransform) {
	MTransform = mTransform;
	NTransform = nTransform;
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