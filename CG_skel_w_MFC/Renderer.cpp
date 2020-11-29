#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <algorithm>

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

bool Renderer::setPixelOn(int x, int y, char color) {
	//printf("SET PIXEL ON %d %d\n", x, y);
	if (x < 0 || x >= m_width || y < 0 || y >= m_width) { 
		return false; 
	}
	string code = get_color(color);
	m_outBuffer[INDEX(m_width - 1, x, y, 0)] = code[0] - '0';
	m_outBuffer[INDEX(m_width - 1, x, y, 1)] = code[1] - '0';
	m_outBuffer[INDEX(m_width - 1, x, y, 2)] = code[2] - '0';
	return true;
}

string Renderer::get_color(char color) {
	switch (color) {
	case 'r':
		return "100";
	case 'g':
		return "010";
	case 'b':
		return "001";
	case 'p':
		return "101";
	case 't':
		return "011";
	case 'w':
		return "111";
	}
	return "000";
}

bool Renderer::pixel_is_on(int x, int y, char color) {
	//printf("PIXEL IS ON\n");
	string code = get_color(color);
	return (m_outBuffer[INDEX(m_width - 1, x, y, 0)] == code[0] - '0' &&
		m_outBuffer[INDEX(m_width - 1, x, y, 1)] == code[1] - '0' &&
		m_outBuffer[INDEX(m_width - 1, x, y, 2)] == code[2] - '0');
}

int Sign(int dxy)
{
	if (dxy < 0) return -1;
	else if (dxy > 0) return 1;
	else return 0;
}

void Renderer::Drawline(int x1, int x2, int y1, int y2, char color, vector<vector<int>>* curr_poly) {
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
			bool ans = setPixelOn(X, Y, color);
			if (ans && curr_poly)(*curr_poly)[X][Y] = 1;
			/*if (curr_poly) {
				if (ans)(*curr_poly)[X][Y] = 1;
				else {
					if (Y < 0) (*curr_poly)[X][m_height] = -1;
					else if (Y >= m_height) (*curr_poly)[X][m_height] = 1;
				}
			}*/
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
			
			bool ans = setPixelOn(X, Y, color);
			if (ans && curr_poly)(*curr_poly)[X][Y] = 1;
			/*if (curr_poly) {
				if (ans)(*curr_poly)[X][Y] = 1;
				else {
					if (Y < 0) { 
						printf("reached here\n");
						printf("height:%d width:%d Y:%d X:%d\n", m_height, m_width, Y, X);
						printf("size:%d\n", (*curr_poly)[X].size()); 
						(*curr_poly)[X][m_height] = -1; 
					}
					else if (Y >= m_height) (*curr_poly)[X][m_height] = 1;
				}
			}*/
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


void Renderer::FillPolygon(char color, vector<vector<int>>* curr_poly){
	printf("FILL POLY\n");
	if (!curr_poly) return;

	/*for (int i = 0; i < m_height; ++i) {
		for (int j = 0; j < m_width; ++j) {
			if ((*curr_poly)[j][i])
				printf(".");
			else
				printf(",");
		}
		printf("\n");
	}*/
	//printf("FILL POLY %d %d %d\n", (int)p1.x , (int)p2.x, (int)p3.x);

	/*int min_x = min(min((int)p1.x, (int)p2.x), (int)p3.x) + 1;
	int max_x = max(max((int)p1.x, (int)p2.x), (int)p3.x);
	int min_y = min(min(p1.y, p2.y), p3.y) + 1;
	int max_y = max(max(p1.y, p2.y), p3.y);*/
	//int min_x = INFINITY, min_y = INFINITY, max_x = -INFINITY, max_y = -INFINITY;
	//printf("y %d %d %d\n", (int)p1.y, (int)p2.y, (int)p3.y);
	/*for (int i = 0; i < m_width; ++i) {
		for (int j = 0; j < m_height; ++j) {
			if ((*curr_poly)[i][j]) {
				min_x = min(min_x, i);
				min_y = min(min_y, j);
				max_x = max(max_x, i);
				max_y = max(max_y, j);
			}
		}
	}
	printf("FILL POLY: %d %d %d %d\n", min_x, min_y, max_x, max_y);*/

	/*double m12 = ((int)p1.y - (int)p2.y != 0) ? ((int)p1.x - (int)p2.x) / ((int)p1.y - (int)p2.y): ((int)p1.x - (int)p2.x);
	double n12 = (int)p1.y - (m12 * (int)p1.x);
	double m13 = ((int)p1.y - (int)p3.y != 0) ? ((int)p1.x - (int)p3.x) / ((int)p1.y - (int)p3.y) : ((int)p1.x - (int)p3.x);
	double n13 = (int)p1.y - (m13 * (int)p1.x);
	double m23 = ((int)p3.y - (int)p2.y != 0) ? ((int)p3.x - (int)p2.x) / ((int)p3.y - (int)p2.y) : ((int)p3.x - (int)p2.x);
	double n23 = (int)p2.y - (m23 * (int)p2.x);

	printf("m 12:%f 13:%f 23:%f\n", m12, m13, m23);
	printf("n 12:%f 13:%f 23:%f\n", n12, n13, n23);*/

	for (int x = 0; x < m_width; ++x) {
		//bool in_poly = false;
		//int y12 = (int)(i * m12 + n12);
		//int y13 = (int)(i * m13 + n13);
		//int y23 = (int)(i * m23 + n23);
		//printf("y 12:%d 13:%d 23:%d\n", y12, y13, y23);
		//int max_y = max(max(y12, y13), y23);
		//int min_y = min(min(y12, y13), y23);
		int min_y = 10000000, max_y = -10000000;
		//printf("min:%d max:%d\n", min_y, max_y);
		for (int y = 0; y < m_height; ++y) {
			if ((*curr_poly)[x][y] == 1) {
				min_y = min(min_y, y);
				max_y = max(max_y, y);
			}
		}

		if (min_y == max_y && (*curr_poly)[x][m_height] < 0) {
			min_y = 0;
		}else if (min_y == max_y && (*curr_poly)[x][m_height] > 0) {
			max_y = m_height - 1;
		} else if (min_y == 10000000 && max_y == -10000000) {
			continue;
		}

		//printf("min:%d max:%d\n", min_y, max_y);
		for (int y = min_y; y < max_y; ++y) {
			//printf("i:%d j:%d\n", i, j);
			//int curr_pixel = (*curr_poly)[x][y];
			/*if (!in_poly && curr_pixel == 1) {
				in_poly = true;
			}
			else if (in_poly && curr_pixel == 0) {
				setPixelOn(i, j, color);
			}
			else if (in_poly && curr_pixel == 1) {
				in_poly = false;
			}*/
			//if (OnBoundary(x, y, curr_poly) || pixel_is_on(x, y, color)) break;
			setPixelOn(x, y, color);

		}
	}


}

bool Renderer::OnBoundary(int x, int y, vector<vector<int>>* curr_poly) {
	//printf("ON BOUNDARY\n");

	return (*curr_poly)[x][y];
}

/*void Renderer::FillPolygon(vector<vector<int>>* curr_poly, int x, int y, char color) {
	printf("FILL POLY %d %d\n", x, y);
	if (!curr_poly || x < 0 || x >= m_width || y < 0 || y >= m_width) return;
	if (OnBoundary(x, y, curr_poly)|| pixel_is_on(x, y, color)) return;
	setPixelOn(x, y, color);
	FillPolygon(curr_poly, x + 1, y, color);
	FillPolygon(curr_poly, x, y + 1, color);
	FillPolygon(curr_poly, x, y - 1, color);
	//FillPolygon(curr_poly, x - 1, y, color);
}*/

void Renderer::DrawTriangles(const vector<vec3>* eye, const vector<vec3>* vertices, const vector<vec3>* normals) {
	printf("DRAW TRIANGLE\n");

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

			vec3 v = vec4t3(temp);
			p[j] = v;

			if (show_normalsV) {
				Drawline(v.x, n.x, v.y, n.y, 'g');
			}
		}
		
		char color = 'p';

		vector<vector<int>> curr_poly(m_width, vector<int>(m_height + 1));

		Drawline(p[0].x, p[1].x, p[0].y, p[1].y, color, &curr_poly);
		Drawline(p[2].x, p[1].x, p[2].y, p[1].y, color, &curr_poly);
		Drawline(p[0].x, p[2].x, p[0].y, p[2].y, color, &curr_poly);

		FillPolygon(color, &curr_poly);

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