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

GLfloat Renderer::pointLight(Light* light, vec3 pixel, vec3 normal, vec4 fraction, vec3 eye) {
	vec3 l = light->place - pixel;
	l = normalize(l);
	normal = normalize(normal);
	GLfloat cos_theta = dot(l, normal);
	GLfloat Id = (cos_theta < 0) ? 0 : light->intensity * cos_theta * fraction[1]; //Kd
	vec3 v = vec3(pixel.x - eye.x, pixel.y - eye.y, pixel.z - eye.z);
	v = normalize(v);
	vec3 r = l - 2 * dot(l, normal) * normal;
	r = normalize(r);
	GLfloat Is = light->intensity * fraction[2] * powf(dot(v, r), fraction[3]);
	if (pixel.x == 256 && pixel.y == 256) {
		printf("l:(%f,%f,%f) normal:(%f,%f,%f) costheta:%f\n", l.x, l.y, l.z, normal.x, normal.y, normal.z, cos_theta);
	}
	return Id + Is;
}

GLfloat Renderer::parallelLight(Light* light, vec4 fraction, vec3 eye, vec3 pixel, vec3 normal){
	vec3 l = light->dir;
	l = normalize(l);
	normal = normalize(normal);
	//GLfloat Ia = light->intensity.x * fraction[0]; //Ka
	GLfloat cos_theta = dot(l, normal);
	GLfloat Id = (cos_theta < 0)? 0 : light->intensity *  cos_theta * fraction[1]; //Kd
	vec3 v = vec3(pixel.x - eye.x, pixel.y - eye.y, pixel.z - eye.z);
	v = normalize(v);
	vec3 r = l - 2 * dot(light->dir, normal) * normal;
	r = normalize(r);
	GLfloat Is = light->intensity * fraction[2] * powf(dot(v, r), fraction[3]);
	return Id + Is;
}

GLfloat Renderer::ambientLight(Light* l, vec4 fraction){
	return l->intensity * fraction[0]; //Ka
}

GLfloat Area(vec2 p1, vec2 p2, vec2 p3) {
	return 0.5 * abs(p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
}

GLfloat depth(int x, int y, vec3 p1, vec3 p2, vec3 p3) {
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

bool Renderer::setPixelOn(int x, int y, vec3 p1, vec3 p2, vec3 p3, vec3 color, bool shade, vec3 eye, vec3 normal, vec4 fraction) {
	//printf("SET PIXEL ON %d %d\n", x, y);
	if (x < 0 || x >= m_width || y < 0 || y >= m_width) { 
		return false; 
	}
	
	// hide
	GLfloat z = depth(x, y, p1, p2, p3);
	if (z >= m_zbuffer[INDEXZ(m_width, x, y)]) {
		return false;
	}
	m_zbuffer[INDEXZ(m_width, x, y)] = z;

	// color
	GLfloat I = 0;
	vec3 curr_color = color;
	if (shade) {
		for (int i = 0; i < lights.size(); i++) {
			if (lights[i]->type == "point")
				I += pointLight(lights[i], vec3(x, y, z), normal, fraction, eye);
			else if (lights[i]->type == "parallel")
				I += parallelLight(lights[i], fraction, eye, vec3(x, y, z), normal);
			else if (lights[i]->type == "ambient")
				I += ambientLight(lights[i], fraction);
			curr_color += lights[i]->color;
			if (x == 0 && y == 0) {
				printf("I:%f\n", I);
			}
		}
		curr_color /= lights.size();
		curr_color *= I;
	}
	
	
	m_outBuffer[INDEX(m_width, x, y, 0)] = curr_color.x;
	m_outBuffer[INDEX(m_width, x, y, 1)] = curr_color.y;
	m_outBuffer[INDEX(m_width, x, y, 2)] = curr_color.z;
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
	//printf("draw line %f %f %f %f\n", x1, x2, y1, y2);
	GLfloat Dx = x2 - x1;
	GLfloat Dy = y2 - y1;

	//# Increments
	int Sx = Sign(Dx);
	int Sy = Sign(Dy);

	//# Segment length
	Dx = abs(Dx);
	Dy = abs(Dy);
	GLfloat D = max(Dx, Dy);

	//# Initial remainder
	GLfloat R = D / 2;

	int X = (int)x1;
	int Y = (int)y1;
	if (Dx > Dy)
	{
		//# Main loop
		for (int I = 0; I < D; I++)
		{
			
			if (save_poly && X >= 0 && X < m_width) {
				(*curr_poly)[X].push_back(Y);
			}
			else if (!save_poly) {
				setPixelOn(X, Y, p1, p2, p2, color);
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
			if (save_poly && X >= 0 && X < m_width) {
				(*curr_poly)[X].push_back(Y);
			}
			else if (!save_poly) {
				setPixelOn(X, Y, p1, p2, p2, color);
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


void Renderer::FillPolygon(vec3 color, vec3 p1, vec3 p2, vec3 p3, vec3 normal, vec4 fraction, vec3 eye){
	//printf("FILL POLY normal(%f,%f,%f) eye(%f,%f,%f)\n", normal.x, normal.y, normal.z, eye.x, eye.y, eye.z);
	
	Drawline(p1, p2, color, true);
	Drawline(p2, p3, color, true);
	Drawline(p3, p1, color, true);

	int min_x = max(min(min(p1.x, p2.x), p3.x) - 5, 0);
	int max_x = min(max(max(p1.x, p2.x), p3.x) + 5, m_width - 1);
	for (int x = min_x; x <= max_x; ++x) {
		if ((*curr_poly)[x].empty()){
			continue;
		}

		int max_y = min(*max_element((*curr_poly)[x].begin(), (*curr_poly)[x].end()), m_height-1);
		int min_y = max(*min_element((*curr_poly)[x].begin(), (*curr_poly)[x].end()), 0);
		
		for (int y = min_y; y <= max_y; ++y) {
			setPixelOn(x, y, p1, p2, p3, color, true, eye, normal, fraction);
		}	
		(*curr_poly)[x].clear();
	}

}

void Renderer::drawSkeleton(const vector<vec3>* vertices) {
	printf("DRAW TRIANGLE\n");

	// draw object
	for (int i = 0; i < vertices->size(); i += 3)
	{
		vec3 p[3];
		for (int j = 0; j < 3; j++) {
			p[j] = vec4t3(STransform * Projection * CTransform * WTransform * MTransform * vec4((*vertices)[i + j]));
		}
		vec3 color = vec3(0, 0, 1);
		Drawline(p[0], p[1], color);
		Drawline(p[1], p[2], color);
		Drawline(p[2], p[0], color);
	}
	
	SetDemoBuffer();
}


void Renderer::DrawTriangles(const vector<vec3>* eye, const vector<vec3>* vertices, vec3 color,const vector<vec3>* normals, const vector<vec3>* vertices_bbox, vec4 fraction, vec3 pos_camera) {
	printf("DRAW TRIANGLE\n");

	// add cam renderer
	for (int j = 0; j < eye->size(); j++) {
		vec4 temp = vec4((*eye)[j]);
		vec3 new_temp = vec4t3(STransform * Projection * CTransform * temp);
		vec3 p1 = new_temp;
		vec3 p2 = new_temp;
		p1.x -= 3; p2.x += 3;
		Drawline(p1, p2, vec3(1,1,1));
		p1 = new_temp;
		p2 = new_temp;
		p1.y -= 3; p2.y += 3;
		Drawline(p1, p2, vec3(1,1,1));
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
				Drawline(p[j], n, vec3(0,1,0));
			}
		}
		
		// draw normals
		center /= 3;
		center = WTransform * MTransform * center;
		vec4 normal_origin = f_normal; // normal that goes out from origin - direction only
		vec4 normal_center = f_normal + center; // normal that goes from center point - to draw normal line on screen

		center = STransform * Projection * CTransform * center;
		normal_center = STransform * Projection * CTransform * normal_center;
		normal_origin = STransform * Projection * CTransform * normal_origin;
		vec3 n = vec4t3(normal_center);
		vec3 n_origin = vec3(normal_origin.x, normal_origin.y, normal_origin.z);
		vec3 c = vec4t3(center);

		vec3 curr_color = color;
		if (!uniform) {
			curr_color = ((i / 3) % 2 == 0)? vec3(0.5, 0.5, 0.5) : color;
			curr_color = ((i / 3) % 2 == 0)? vec3(0.5, 0.5, 0.5) : color;
		}

		if (n_origin.z > 0) {
			FillPolygon(curr_color, p[0], p[1], p[2], n_origin, fraction, pos_camera);
		}
		if (show_normalsF){
			Drawline(c, n, vec3(0,1,1));
		}
		
	}

	if(lights.size() >= 2)
		Drawline(lights[1]->place, vec3(256, 256, -BIG_NUMBER), vec3(1, 0, 0));

	// draw bounding box
	if (!bbox) return;

	for (int i = 0; i < vertices_bbox->size(); i += 2)
	{
		vec3 p[2];

		for (int j = 0; j < 2; j++) {
			vec4 temp = WTransform * MTransform * vec4((*vertices_bbox)[i + j]);
			temp = STransform * Projection * CTransform * temp;
			vec3 v = vec4t3(temp);
			p[j] = v;

		}

		Drawline(p[0], p[1], vec3(1,0,0));
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
			m_zbuffer[INDEXZ(m_width, i, j)] = BIG_NUMBER;
		}
}

void Renderer::SetCameraMatrices(const mat4& cTransform, const mat4& projection) {
	CTransform = cTransform;
	Projection = projection;
}
void Renderer::SetScreenTransform() {
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

void Renderer::SetFlags(bool bbox, bool show_normalsV, bool show_normalsF, bool uniform) {
	this->bbox = bbox;
	this->show_normalsF = show_normalsF;
	this->show_normalsV = show_normalsV;
	this->uniform = uniform;
}

void Renderer::Init() {
	ClearColorBuffer();
	ClearDepthBuffer();
}