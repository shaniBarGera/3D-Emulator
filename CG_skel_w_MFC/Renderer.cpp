#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <algorithm>

#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)
#define KERNEL_SIZE 3

Renderer::Renderer() :m_width(512), m_height(512), real_m_width(512), real_m_height(512)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
	getGaussian();
}
Renderer::Renderer(int width, int height) :m_width(width), m_height(height), real_m_width(width), real_m_height(height)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
	getGaussian();
}

Renderer::~Renderer(void)
{
}

void Renderer::CreateBuffers(int width, int height)
{
	printf("CREATE BUFFERS\n");
	m_width = width;
	m_height = height;

	
	if (antialiasing) {
		m_width *= KERNEL_SIZE;
		m_height *= KERNEL_SIZE;
		real_m_width = width;
		real_m_height = height;
		
	}

	CreateOpenGLBuffer(); //Do not remove this line.

	if(antialiasing) real_m_outBuffer = new float[3 * real_m_width * real_m_height];
	m_outBuffer = new float[3 * m_width * m_height];
	m_blurBuffer = new GLfloat[3 * m_width * m_height];
	m_blurBuffer_x = new GLfloat[3 * m_width * m_height];
	m_brightBuffer = new GLfloat[3 * m_width * m_height];
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

vec3 Renderer::pointLight(Light* light, vec3 pixel, vec3 normal, vec4 fraction, vec3 eye, vec3 screen_pixel) {
	
	GLfloat Ia = light->intensity * fraction[0]; //Ka
	vec3 l = normalize(light->place - pixel);
	normal = normalize(normal);
	GLfloat cos_theta = dot(l, normal);
	GLfloat Id = (cos_theta < 0) ? 0 : light->intensity * cos_theta * fraction[1]; //Kd
	vec3 v = eye - pixel;
	v = normalize(v);
	vec3 r = l - 2 * dot(l, normal) * normal;
	r = normalize(r);
	GLfloat Is = light->intensity * fraction[2] * powf(dot(v, r), fraction[3]);

	return vec3(Ia, Id, Is);
}

vec3 Renderer::parallelLight(Light* light, vec4 fraction, vec3 eye, vec3 pixel, vec3 normal){
	vec3 l = normalize(light->place);
	normal = normalize(normal);
	GLfloat Ia = light->intensity * fraction[0]; //Ka
	GLfloat cos_theta = dot(l, normal);
	GLfloat Id = (cos_theta < 0)? 0 : light->intensity *  cos_theta * fraction[1]; //Kd
	vec3 v = eye - pixel;
	v = normalize(v);
	vec3 r = l - 2 * dot(light->place, normal) * normal;
	r = normalize(r);
	GLfloat Is = light->intensity * fraction[2] * powf(dot(v, r), fraction[3]);
	return vec3(Ia, Id, Is);
}

vec3 Renderer::ambientLight(Light* l, vec4 fraction){
	return vec3(l->intensity * fraction[0]); //Ka
}

GLfloat Area(vec2 p1, vec2 p2, vec2 p3) {
	return 0.5 * abs(p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
}

vec3 getWeights(int x, int y, vec3 p1, vec3 p2, vec3 p3) {
	vec2 a = vec2(x, y);
	vec2 b = vec2(p1.x, p1.y);
	vec2 c = vec2(p2.x, p2.y);
	vec2 d = vec2(p3.x, p3.y);
	GLfloat a1 = Area(a, c, d);
	GLfloat a2 = Area(a, b, d);
	GLfloat a3 = Area(a, b, c);
	GLfloat sum_a = a1 + a2 + a3;
	return vec3(a1/ sum_a, a2/ sum_a, a3/sum_a);
}

bool Renderer::setPixelOn(vec3 pixel, vec3 color) {
	//printf("SET PIXEL ON %d %d\n", x, y);
	int x = pixel.x;
	int y = pixel.y;
	if (x < 0 || x >= m_width || y < 0 || y >= m_width) { 
		return false; 
	}
	if (pixel.z <= m_zbuffer[INDEXZ(m_width, x, y)]) {
		return false;
	}

	m_zbuffer[INDEXZ(m_width, x, y)] = pixel.z;
	
	m_outBuffer[INDEX(m_width, x, y, 0)] = color.x;
	m_outBuffer[INDEX(m_width, x, y, 1)] = color.y;
	m_outBuffer[INDEX(m_width, x, y, 2)] = color.z;

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
				setPixelOn(vec3(X, Y, BIG_NUMBER), color);
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
				setPixelOn(vec3(X, Y, BIG_NUMBER), color);
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


void Renderer::FillPolygon(mat3 color, vec3 p1, vec3 p2, vec3 p3, vec3 normal1, vec3 normal2, vec3 normal3, vec3 normal_f, vec4 fraction, vec3 eye, mat3 world_ps)
{
	//printf("FILL POLY\n");
	Drawline(p1, p2, color[0], true);
	Drawline(p2, p3, color[0], true);
	Drawline(p3, p1, color[0], true);

	int min_x = max(min(min(p1.x, p2.x), p3.x) - 5, 0);
	int max_x = min(max(max(p1.x, p2.x), p3.x) + 5, m_width - 1);
	for (int x = min_x; x <= max_x; ++x) {
		if ((*curr_poly)[x].empty()){
			continue;
		}

		int max_y = min(*max_element((*curr_poly)[x].begin(), (*curr_poly)[x].end()), m_height-1);
		int min_y = max(*min_element((*curr_poly)[x].begin(), (*curr_poly)[x].end()), 0);
		
		for (int y = min_y; y <= max_y; ++y) {
			vec3 weights = getWeights(x, y, p1, p2, p3);
			vec3 world_pixel = transpose(world_ps) * weights; 
			vec3 curr_color = vec3(0);
			if (fogefect) {
				float fog_maxdist = 2;
				float fog_mindist = -2;
				vec3 fog_colour = vec3(0.4);
				float fog_factor = (fog_maxdist - world_pixel.z) / (fog_maxdist - fog_mindist);
				fog_colour *= fog_factor;
				curr_color -= fog_colour;
			}
			
			vec3 I = 0; vec3 I1 = 0; vec3 I2 = 0; vec3 I3 = 0;
			if (shade == 'f') {
				for (int i = 0; i < lights.size(); i++) {
					vec3 curr_I;
					if (lights[i]->type == "point")
						curr_I = pointLight(lights[i], world_pixel, normal_f, fraction, eye, vec3(x, y, world_pixel.z));
					else if (lights[i]->type == "parallel")
						curr_I = parallelLight(lights[i], fraction, eye, world_pixel, normal_f);
					else if (lights[i]->type == "ambient")
						curr_I = ambientLight(lights[i], fraction);

					// split color
					mat3 temp_color = color;
					curr_I.x = (curr_I.x < 0) ? 0 : curr_I.x;
					curr_I.y = (curr_I.x < 0) ? 0 : curr_I.y;
					curr_I.z = (curr_I.x < 0) ? 0 : curr_I.z;
					temp_color[0] *= curr_I.x;
					temp_color[1] *= curr_I.y;
					temp_color[2] *= curr_I.z;
					vec3 temp_curr_color = temp_color[0] + temp_color[1] + temp_color[2];
					temp_curr_color *= lights[i]->color;
					curr_color += temp_curr_color;
					/*for (int j = 0; j < 3; j++) {
						curr_I[j] = (curr_I[j] < 0) ? 0 : curr_I[j];
						curr_color += lights[i]->color * curr_I[j] * color[j];
						I += curr_I[j];
					}*/
				}
				
			}
			else if (shade == 'g') {
				for (int i = 0; i < lights.size(); i++) {
					vec3 curr_I;
					if (lights[i]->type == "point") {
						I1 += pointLight(lights[i], world_ps[0], normal1, fraction, eye, p1);
						I2 += pointLight(lights[i], world_ps[1], normal2, fraction, eye, p2);
						I3 += pointLight(lights[i], world_ps[2], normal3, fraction, eye, p3);
						curr_I = weights.x * I1 + weights.y * I2 + weights.z * I3;
					}
					else if (lights[i]->type == "parallel") {
						I1 += parallelLight(lights[i], fraction, eye, world_ps[0], normal1);
						I2 += parallelLight(lights[i], fraction, eye, world_ps[1], normal2);
						I3 += parallelLight(lights[i], fraction, eye, world_ps[2], normal3);
						curr_I = weights.x * I1 + weights.y * I2 + weights.z * I3;
					}
					else if (lights[i]->type == "ambient")
						curr_I = ambientLight(lights[i], fraction);
					
					/*GLfloat curr_I_sum = curr_I.x + curr_I.y + curr_I.z;
					curr_I = (curr_I_sum < 0) ? 0 : curr_I_sum;
					curr_color += lights[i]->color * curr_I_sum;
					I += curr_I_sum;*/
					mat3 temp_color = color;
					curr_I.x = (curr_I.x < 0) ? 0 : curr_I.x;
					curr_I.y = (curr_I.x < 0) ? 0 : curr_I.y;
					curr_I.z = (curr_I.x < 0) ? 0 : curr_I.z;
					temp_color[0] *= curr_I.x;
					temp_color[1] *= curr_I.y;
					temp_color[2] *= curr_I.z;
					vec3 temp_curr_color = temp_color[0] + temp_color[1] + temp_color[2];
					temp_curr_color *= lights[i]->color;
					curr_color += temp_curr_color;
				}
			}
			else if (shade == 'p') {
				for (int i = 0; i < lights.size(); i++) {
					vec3 curr_I;
					if (lights[i]->type == "point") {
						vec3 normal = weights.x * normal1 + weights.y * normal2 + weights.z * normal3;
						curr_I = pointLight(lights[i], world_pixel, normal, fraction, eye, vec3(x, y, world_pixel.z));
					}
					else if (lights[i]->type == "parallel") {
						vec3 normal = weights.x * normal1 + weights.y * normal2 + weights.z * normal3;
						curr_I = parallelLight(lights[i], fraction, eye, world_pixel, normal);
					}
					else if (lights[i]->type == "ambient")
						curr_I = ambientLight(lights[i], fraction);

					/*GLfloat curr_I_sum = curr_I.x + curr_I.y + curr_I.z;
					curr_I = (curr_I_sum < 0) ? 0 : curr_I_sum;
					curr_color += lights[i]->color * curr_I_sum;
					I += curr_I_sum;*/
					mat3 temp_color = color;
					curr_I.x = (curr_I.x < 0) ? 0 : curr_I.x;
					curr_I.y = (curr_I.x < 0) ? 0 : curr_I.y;
					curr_I.z = (curr_I.x < 0) ? 0 : curr_I.z;
					temp_color[0] *= curr_I.x;
					temp_color[1] *= curr_I.y;
					temp_color[2] *= curr_I.z;
					vec3 temp_curr_color = temp_color[0] + temp_color[1] + temp_color[2];
					temp_curr_color *= lights[i]->color;
					curr_color += temp_curr_color;
					
				}
			}
			setPixelOn(vec3(x, y, world_pixel.z), curr_color);
		}	
		(*curr_poly)[x].clear();
	}

}

void Renderer::drawSkeleton(const vector<vec3>* vertices) {
	//printf("DRAW TRIANGLE\n");

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

/*void Renderer::getGaussian()
{
	GLfloat sigma = 10.0;
	GLfloat sum = 0.0;
	int i, j;

	for (i = 0; i <= FILTER_SIZE; i++) {
		for (j = 0; j < FILTER_SIZE; j++) {
			int i1 = i - FILTER_HALF;
			int j1 = j - FILTER_HALF;
			filter[i][j] = exp(-(i1 * i1 + j1 * j1) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
			sum += filter[i][j];
		}
	}

	printf("filter:\n");
	for (i = 0; i < FILTER_SIZE; i++) {
		for (j = 0; j < FILTER_SIZE; j++) {
			filter[i][j] /= sum;
			printf("%f ", filter[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}*/

void Renderer::getGaussian()
{
	GLfloat sigma = 10.0;
	GLfloat sum = 0.0;
	int i, j;

	for (i = 0; i <= FILTER_SIZE; i++) {
		int i1 = i - FILTER_HALF;
		filter[i] = exp(-(i1 * i1) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
		sum += filter[i];
	}

	printf("filter:\n");
	for (i = 0; i < FILTER_SIZE; i++) {
		filter[i] /= sum;
		printf("%f ", filter[i]);
	}
	printf("\n");
}

/*void Renderer::applyFilter() {
	int d, i, j, h, w;
	//printf("bright:%f\n", m_brightBuffer[INDEX(m_width, 0, 0, 0)]);
	for (int d = 0; d < 3; d++) {
		for (int i = FILTER_HALF; i < m_width - FILTER_HALF; i++) {
			for (int j = FILTER_HALF; j < m_height - FILTER_HALF; j++) {
				for (int h = 0; h < FILTER_SIZE; h++) {
					for (int w = 0 ; w < FILTER_SIZE; w++) {
						if (h + i - FILTER_HALF >= m_width || w + j - FILTER_HALF >= m_height ||
							h + i - FILTER_HALF < 0 || w + j - FILTER_HALF < 0||
							i - FILTER_HALF < 0 || j - FILTER_HALF < 0||
							i - FILTER_HALF >= m_width || j - FILTER_HALF > m_height) {
							printf("h:%d i:%d j:%d w:%d\n", h, i, j, w);
						}
						//printf("h:%d i:%d j:%d w:%d\n", h, i, j, w);
						GLfloat f = filter[h][w];
						//printf("1\n");
						int i1 = h + i - FILTER_HALF; 
						int j1 = w + j - FILTER_HALF;
						//printf("1 i-half:%d j-half:%d d:%d\n", i1, j1, d);
						GLfloat bright = m_brightBuffer[INDEX(m_width, i1, j1, d)];
						//printf("2");
						m_blurBuffer[INDEX(m_width, i, j, d)] += f * bright;
						//printf("3");
					}
				}
			}
		}
	}
}*/

void Renderer::applyFilter() {
	int d, i, j, h, w;
	//printf("bright:%f\n", m_brightBuffer[INDEX(m_width, 0, 0, 0)]);
	for (int d = 0; d < 3; d++) {
		for (int i = FILTER_HALF; i < m_width - FILTER_HALF; i++) {
			for (int j = FILTER_HALF; j < m_height - FILTER_HALF; j++) {
				for (int h = 0; h < FILTER_SIZE; h++) {
					GLfloat f = filter[h];
					//printf("1\n");
					int i1 = h + i - FILTER_HALF;
					//printf("1 i-half:%d j-half:%d d:%d\n", i1, j1, d);
					GLfloat bright = m_brightBuffer[INDEX(m_width, i1, j, d)];
					//printf("2");
					m_blurBuffer_x[INDEX(m_width, i, j, d)] += f * bright;
					//printf("3");
				}
			}
		}
		for (int i = FILTER_HALF; i < m_width - FILTER_HALF; i++) {
			for (int j = FILTER_HALF; j < m_height - FILTER_HALF; j++) {
				for (int h = 0; h < FILTER_SIZE; h++) {
					GLfloat f = filter[h];
					//printf("1\n");
					int j1 = h + j - FILTER_HALF;
					//printf("1 i-half:%d j-half:%d d:%d\n", i1, j1, d);
					GLfloat bx = m_blurBuffer_x[INDEX(m_width, i, j1, d)];
					//printf("2");
					m_blurBuffer[INDEX(m_width, i, j, d)] += f * bx;
					//printf("3");
				}
			}
		}
	}
}

void Renderer::drawCameras(const vector<vec3>* eye) {
	// draw camera
	for (int j = 0; j < eye->size(); j++) {
		vec4 temp = vec4((*eye)[j]);
		vec3 new_temp = vec4t3(STransform * Projection * CTransform * temp);
		vec3 p1 = new_temp;
		vec3 p2 = new_temp;
		p1.x -= 3; p2.x += 3;
		Drawline(p1, p2, vec3(1, 1, 1));
		p1 = new_temp;
		p2 = new_temp;
		p1.y -= 3; p2.y += 3;
		Drawline(p1, p2, vec3(1, 1, 1));
	}
}

void Renderer::blur() {
	printf("BLUR\n");
	// blur
	if (!blureffect) return;
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			for (int c = 0; c < 3; c++) {
				m_brightBuffer[INDEX(m_width, i, j, c)] = 
					(m_outBuffer[INDEX(m_width, i, j, c)] > threshold) ? m_outBuffer[INDEX(m_width, i, j, c)] - threshold : 0;
			}
		}
	}
	for (int i = 0; i < 5; i++) {
		applyFilter();
	}
	printf("APPLIED FILTER\n");
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			for (int c = 0; c < 3; c++) {
				m_outBuffer[INDEX(m_width, i, j, c)] += m_blurBuffer[INDEX(m_width, i, j, c)];
				//m_outBuffer[INDEX(m_width, i, j, c)] = m_brightBuffer[INDEX(m_width, i, j, c)];
				//m_outBuffer[INDEX(m_width, i, j, c)] =  m_blurBuffer[INDEX(m_width, i, j, c)];
			}
		}
	}
}

void Renderer::anti_aliasing() {
	if (!antialiasing) return;

	for (int i = 0; i < m_width; i += KERNEL_SIZE) {
		for (int j = 0; j < m_height; j += KERNEL_SIZE)
		{
			for (int c = 0; c < 3; c++) {
				int i1 = i / KERNEL_SIZE;
				int j1 = j / KERNEL_SIZE;
				real_m_outBuffer[INDEX(real_m_width, i1, j1, c)] = 0.0;
				for (int k = 0; k < KERNEL_SIZE; k++) {
					for (int w = 0; w < KERNEL_SIZE; w++) {
						int i2 = i + k;
						int j2 = j + w;
						real_m_outBuffer[INDEX(real_m_width, i1, j1, c)] +=
							(m_outBuffer[INDEX(m_width, i2, j2, c)]);
					}
				}
				real_m_outBuffer[INDEX(real_m_width, i1, j1, c)] /= 9.0;
			}
		}
	}
}

void Renderer::drawBBox(const vector<vec3>* vertices_bbox) {
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

		Drawline(p[0], p[1], vec3(1, 0, 0));
	}
}

void Renderer::DrawTriangles(const vector<vec3>* vertices, mat3 color,const vector<vec3>* normals, 
	vec4 fraction, vec3 pos_camera, vector<vector<vec3>> avg_normals) {

	// draw object
	for (int i = 0; i < vertices->size(); i+=3)
	{
		vec3 p[3];
		mat3 world_p;
		vec4 center(0);
		vec4 f_normal;
		vec3 norm_per_v[3];
		for (int j = 0; j < 3; j++) {
			center += vec4((*vertices)[i + j]);
			vec4 temp = WTransform * MTransform * vec4((*vertices)[i + j]);
			vec4 normal = NWTransform * NTransform * vec4((*normals)[i + j]) * 0.2;
			
			vec3 avg_n = avg_normals[j+i][0];
			vec4 avg_normal = NWTransform * NTransform * vec4(avg_n);

			normal.w = 0;
			
			if (j == 0) {
				f_normal = normal;
			}
			
			normal += temp;

			avg_normal = STransform * Projection * CTransform * avg_normal;
			normal = STransform * Projection * CTransform * normal;
			vec3 n = vec4t3(normal);
			vec3 avg_norm = vec4t3(avg_normal);
			
			temp = Projection* CTransform* temp;
			world_p[j] = vec4t3(temp);
			temp = STransform * temp;
			p[j] = vec4t3(temp);

			if (show_normalsV) {
				Drawline(p[j], n, vec3(0,1,0));
			}

			norm_per_v[j]=normalize(avg_norm);
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

		mat3 curr_color = color;
		if (!uniform) {
			curr_color = ((i / 3) % 2 == 0)? mat3(0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5) : color;
			curr_color = ((i / 3) % 2 == 0)? mat3(0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5) : color;
		}

		FillPolygon(curr_color, p[0], p[1], p[2], norm_per_v[0], norm_per_v[1], norm_per_v[2], n_origin, fraction, pos_camera, world_p);

		if (show_normalsF)
			Drawline(c, n, vec3(0,1,1));
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

			m_blurBuffer[INDEX(m_width, i, j, 0)] = 0;
			m_blurBuffer[INDEX(m_width, i, j, 1)] = 0;
			m_blurBuffer[INDEX(m_width, i, j, 2)] = 0;

			m_blurBuffer_x[INDEX(m_width, i, j, 0)] = 0;
			m_blurBuffer_x[INDEX(m_width, i, j, 1)] = 0;
			m_blurBuffer_x[INDEX(m_width, i, j, 2)] = 0;

			m_brightBuffer[INDEX(m_width, i, j, 0)] = 0;
			m_brightBuffer[INDEX(m_width, i, j, 1)] = 0;
			m_brightBuffer[INDEX(m_width, i, j, 2)] = 0;
		}
	}
}


void Renderer::reshape(int width, int height) {
	CreateBuffers(width, height);
}

void Renderer::setBuffer() {
	if(antialiasing)
		CreateBuffers(m_width, m_height);
	else
		CreateBuffers(m_width/KERNEL_SIZE, m_height/KERNEL_SIZE);
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
	if (antialiasing) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, real_m_width, real_m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glViewport(0, 0, real_m_width, real_m_height);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glViewport(0, 0, m_width, m_height);
	}
	
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	if(antialiasing) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, real_m_width, real_m_height, GL_RGB, GL_FLOAT, real_m_outBuffer);
	else glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
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
			m_zbuffer[INDEXZ(m_width, i, j)] = -BIG_NUMBER;
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