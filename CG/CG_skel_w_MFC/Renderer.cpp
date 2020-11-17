#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

GLfloat get_max_of_x(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (size_t i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x >= max) max = (*vertices)[i].x;
	}
	return max;
}

GLfloat get_max_of_y(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].y >= max) max = (*vertices)[i].y;
	}
	return max;
}

GLfloat get_min_of_x(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].x <= min) min = (*vertices)[i].x;
	}

	return min;
}

GLfloat get_min_of_y(const vector<vec3>* vertices) {
	GLfloat min = 10000;
	for (int i = 0; i < vertices->size() - 1; i++) {
		if ((*vertices)[i].y <= min) min = (*vertices)[i].y;
	}
	return min;
}

int normal(GLfloat input, GLfloat output_start, GLfloat output_end, GLfloat input_start, GLfloat input_end) {
	return int(output_start + ((output_end - output_start) * (input - input_start)) / (input_end - input_start));
}

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

void Renderer::setPixelOn(int x, int y) {
	m_outBuffer[INDEX(m_width - 1, x, y, 0)] = 1;
	m_outBuffer[INDEX(m_width - 1, x, y, 1)] = 0;
	m_outBuffer[INDEX(m_width - 1, x, y, 2)] = 0;
}

int Sign(int dxy)
{
	if (dxy < 0) return -1;
	else if (dxy > 0) return 1;
	else return 0;
}

void Renderer::Drawline(int x1, int x2, int y1, int y2) {
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
			setPixelOn(X, Y);
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
			setPixelOn(X, Y);
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
	vec4 a = vec4((m_width-1)/(get_max_of_x(vertices) - get_min_of_x(vertices))  , 0, 0, 0);
	vec4 b = vec4(0, (m_height-1)/(get_max_of_y(vertices) - get_min_of_y(vertices)) , 0, 0);
	vec4 c = vec4(0, 0, 1, 0);
	vec4 d = vec4(0, 0, 0, 1);
	WTransform = mat4(a, b, c, d);
	//vec4 e = vec4(1, 0, 0, -(get_min_of_x(vertices)));
	//vec4 f = vec4(0, 1, 0, -(get_min_of_y(vertices)));
	//vec4 g = vec4(0, 0, 1, 0);
	//vec4 h = vec4(0, 0, 0, 1);
	//mat4 _world_transform = mat4(e, f, g, h);
	//GLfloat max_x = get_max_of_x(vertices);
	//GLfloat max_y = get_max_of_y(vertices);
	//GLfloat min_x = get_min_of_x(vertices);
	//GLfloat min_y = get_min_of_y(vertices);
	
	for (int i = 0; i < vertices->size()-1; i+=3)
	{
		//vec4 v = ((*vertices)[i], 0);
		
		int x[3] = { 0 };
		int y[3] = { 0 };
		for (int j = 0; j < 3; j++) {
			vec4 temp = WTransform*OTransform*vec4((*vertices)[i + j]);

			x[j] = 0.1*temp.x/temp.w;
			y[j] = 0.1*temp.y / temp.w;
			//x[j] = normal((*vertices)[i + j].x, 0, m_width - 1, min_x, max_x);
			//y[j] = normal((*vertices)[i + j].y, 0, m_height - 1, min_y, max_y);
			// translate v to (x,y,z,1)
			// v = Projection * CTransfrom^(-1) * OTransfrom * Vtranslated
			
			// lookAt(eye, curr_v, up)
			//x[j] = v.x;
			//y[j] = v.y;
		}

		Drawline(x[0], x[1], y[0], y[1]);
		Drawline(x[2], x[1], y[2], y[1]);
		Drawline(x[0], x[2], y[0], y[2]);
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
	Projection = projection;
}
void Renderer::SetObjectMatrices(const mat4& oTransform, const mat3& nTransform) {
	OTransform = oTransform;
	NTransform = nTransform;


}

void Renderer::Init() {
	ClearColorBuffer();
	ClearDepthBuffer();
	vec4 a = vec4(1.0, 0.0, 0.0, 0.0);
	vec4 b = vec4(0.0, 1.0, 0.0, 0.0);
	vec4 c = vec4(0.0, 0.0, 1.0, 0.0);
	vec4 d = vec4(0.0, 0.0, 0.0, 1.0);
	CTransform = mat4(a, b, c, d);
	OTransform = mat4(a, b, c, d);
	Projection = mat4(a, b, c, d);
	//NTransform = mat4(a, b, c, d);
}