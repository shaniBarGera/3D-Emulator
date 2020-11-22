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

void Renderer::setPixelOn(int x, int y, char color) {
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
	
	switch (color) {
	case 'r' :
		m_outBuffer[INDEX(m_width - 1, x, y, 0)] = 1;
		break;
	case 'g':
		m_outBuffer[INDEX(m_width - 1, x, y, 1)] = 1;
		break;
	case 'b':
		m_outBuffer[INDEX(m_width - 1, x, y, 2)] = 1;
		break;
	case 'p':
		m_outBuffer[INDEX(m_width - 1, x, y, 0)] = 1;
		m_outBuffer[INDEX(m_width - 1, x, y, 2)] = 1;
		break;
	}
}

int Sign(int dxy)
{
	if (dxy < 0) return -1;
	else if (dxy > 0) return 1;
	else return 0;
}

void Renderer::Drawline(int x1, int x2, int y1, int y2, char color) {
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


void Renderer::DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals) {
	printf("DRAW TRIANGLE\n");

	vector<vec3> v;
	v.insert(end(v), begin(*vertices), end(*vertices));
	//v.insert(end(v), begin(*normals), end(*normals));

	for (int i = 0; i < v.size()-1; i+=3)
	{
		
		GLfloat x[3] = { 0 };
		GLfloat y[3] = { 0 };

		for (int j = 0; j < 3; j++) {
			vec4 temp = WTransform * MTransform * vec4(v[i + j]);

			// camera transform
			mat4 c = CTransform;
			temp = Projection * c * temp;
		
			// final result
			x[j] = temp.x/temp.w;
			y[j] = temp.y/temp.w;
		}

		char color = 'p';
		if (bbox && vertices->size() - i <= 36 && vertices->size() - i >= 0)
			color = 'r'; 
		
		/*int diff = vertices->size() - i;
		if (diff <= 0) {
			color = 'g';
		}*/
		
		
		Drawline(x[0], x[1], y[0], y[1], color);
		Drawline(x[2], x[1], y[2], y[1], color);
		Drawline(x[0], x[2], y[0], y[2], color);

	}

	if (!show_normalsF) return;

	for (int i = 0; i < normals->size() - 1; i += 2)
	{

		GLfloat x[2] = { 0 };
		GLfloat y[2] = { 0 };

		for (int j = 0; j < 2; j++) {
			vec4 temp = WTransform * MTransform * vec4((*normals)[i + j]);

			// camera transform
			mat4 c = CTransform;
			temp = Projection * c * temp;

			// final result
			x[j] = temp.x / temp.w;
			y[j] = temp.y / temp.w;
		}

		char color = 'g';
		//setPixelOn(x[0], y[0], color);
		//setPixelOn(x[1], y[1], color);
		Drawline(x[0], x[1], y[0], y[1], color);
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
	m_width = width;
	m_height = height;
	CreateLocalBuffer();
	//SwapBuffers();
}

void Renderer::CreateLocalBuffer() {
	delete[] m_outBuffer;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width * m_height];
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

void Renderer::SetCameraTransform(const mat4& cTransform) {
	CTransform = cTransform;
	
}
void Renderer::SetProjection(const mat4& projection) {
	printf("SET PROJECTION\n");
	Projection = projection;
	Projection[0][3] += m_width / 2;
	Projection[1][3] += m_height / 2;
}
void Renderer::SetObjectMatrices(GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y,
							const mat4& mTransform, const mat4& wTransform, const mat3& nTransform) {
	MTransform = mTransform;
	NTransform = nTransform;
	WTransform = wTransform;

	WTransform[0][0] = (m_width - 1) / (max_x - min_x);
	WTransform[1][1] = (m_height - 1) / (max_y - min_y);
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