#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

GLfloat get_max_of_x(const vector<vec3>* vertices) {
	GLfloat max = 0;
	for (int i = 0; i < vertices->size() - 1; i++) {
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
		m_outBuffer[INDEX(m_width,256,i,0)]=1;	m_outBuffer[INDEX(m_width,256,i,1)]=0;	m_outBuffer[INDEX(m_width,256,i,2)]=0;

	}
	//horizontal line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,i,256,0)]=1;	m_outBuffer[INDEX(m_width,i,256,1)]=0;	m_outBuffer[INDEX(m_width,i,256,2)]=1;

	}
}





void Renderer::DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals) {
	//clean bufer
	//for (int i = 0; i < m_width; i++)
	//	for (int j = 0; j < m_height; j++) {
	//		m_outBuffer[INDEX(m_width, i, j, 0)] = 0;
	//	}
	
	GLfloat max_x = get_max_of_x(vertices);
	GLfloat max_y = get_max_of_y(vertices);
	GLfloat min_x = get_min_of_x(vertices);
	GLfloat min_y = get_min_of_y(vertices);
	for (int i = 0; i < vertices->size()-1; i++)
	{
		int x1 = normal((*vertices)[i].x, 0, m_width-1, min_x, max_x);
		int x2 = normal((*vertices)[i+1].x, 0, m_width-1, min_x, max_x);
		int y1 = normal((*vertices)[i].y, 0, m_height-1, min_y, max_y);
		int y2 = normal((*vertices)[i + 1].y, 0, m_height-1, min_y, max_y);
		//int m = int((y2-y1)/(x2-x1));
		//printf("%d %d %d %d\n",x1,x2,y1,m);
		//printf("%d\n", m);
		m_outBuffer[INDEX(m_width, x1, y1, 1)] = 1;
		m_outBuffer[INDEX(m_width, x2, y2, 1)] = 1;
		
		//int m_new = 2 * (y2 - y1);
		//int slope_error_new = m_new - (x2 - x1);
		//for (int x = x1, y = y1; x <= x2; x++)
		//{
		//	m_outBuffer[INDEX(m_width, x, y, 0)] = 1;

			// Add slope to increment angle formed 
			//slope_error_new += m_new;

			// Slope error reached limit, time to 
			// increment y and update slope error. 
			//if (slope_error_new >= 0)
			//{
			//	y++;
			//	slope_error_new -= 2 * (x2 - x1);
			//}
		//}
		
	}
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

