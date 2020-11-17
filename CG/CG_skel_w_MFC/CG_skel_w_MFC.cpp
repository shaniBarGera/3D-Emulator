// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include "InputDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include <string>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define CAM_ADD 1
#define CAM_TRANS 2
#define CAM_REND 3
#define CAM_ACTIVE 4
#define CAM_FOCUS 5

#define FILE_OPEN 1

#define MAIN_DEMO 1
#define MAIN_ABOUT 2
#define MAIN_PRIM 3
#define MAIN_STEP 4

#define MODEL_ACTIVE 1
#define MODEL_NORMAL_V 2
#define MODEL_NORMAL_F 3
#define MODEL_BBOX 4
#define MODEL_ROTATE 5


Scene* scene;
Renderer* renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;

//--------------------------------------------------------------------------
// Helpers

string dialogBox() {
	CCmdDialog dlg;
	if (dlg.DoModal() == IDOK) {
		return dlg.GetCmd();
	}
}


//----------------------------------------------------------------------------
// Callbacks

void display(void)
{
	printf("DISPLAY\n");
	//Call the scene and ask it to draw itself
	//scene* new_scene = new scene(fileName);
	scene->draw(); //CHANGE
}

void reshape(int width, int height)
{
	//update the renderer's buffers
	printf("RESHAPE\n");

	renderer->reshape(width, height);
}

void keyboard(unsigned char key, int x, int y)
{
	printf("KEYBOARD\n");
	switch (key) {
	case 033:
		printf("EXIT\n");
		exit(EXIT_SUCCESS);
		break;
	case '+':
		printf("PLUS\n");
		scene->zoomIn();
		break;
	case '-':
		printf("MINUS\n");
		scene->zoomOut();
	}
}

void catchKey(int key, int x, int y) {
	printf("CATCH KEY\n");
	switch (key) {
	case GLUT_KEY_LEFT:
		scene->move();
		break;
	case GLUT_KEY_RIGHT:
		scene->move();
		break;
	case GLUT_KEY_UP:
		scene->move();
		break;
	case GLUT_KEY_DOWN:
		scene->move();
		break;
	}
}


void mouse(int button, int state, int x, int y)
{
	printf("MOUSE\n");
	//button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
	//state = {GLUT_DOWN,GLUT_UP}

	//set down flags
	switch (button) {
	case GLUT_LEFT_BUTTON:
		printf("LEFT MOUSE\n");
		lb_down = (state == GLUT_UP) ? 0 : 1;
		break;
	case GLUT_RIGHT_BUTTON:
		printf("RIGHT MOUSE\n");
		rb_down = (state == GLUT_UP) ? 0 : 1;
		break;
	case GLUT_MIDDLE_BUTTON:
		printf("MIDDLE MOUSE\n");
		mb_down = (state == GLUT_UP) ? 0 : 1;
		break;
	}

	// add your code
	//reshape(200, 200);
	//display();
}

void motion(int x, int y)
{
	printf("MOTION\n");
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = y - last_y;

	// update last x,y
	last_x = x;
	last_y = y;
	//reshape(800, 300);

	scene->scale(dx, dy);
}

void fileMenu(int id)
{
	switch (id)
	{
	case FILE_OPEN:
		CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
		if (dlg.DoModal() == IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
		}
		break;
	}
}

void mainMenu(int id)
{
	switch (id)
	{
	case MAIN_DEMO:
		scene->drawDemo();
		break;
	case MAIN_ABOUT:
		AfxMessageBox(_T("Computer Graphics"));
		break;
	case MAIN_PRIM:
		scene->addPrim();
		break;
	case MAIN_STEP:
		scene->step = stoi(dialogBox());
	}
}

void camMenu(int id) {
	switch (id)
	{
	case CAM_ADD:
		scene->addCam(dialogBox());
		break;
	case CAM_REND:
		scene->render();
		break;
	case CAM_FOCUS:
		scene->focus();
		break;
	case CAM_ACTIVE:
		scene->activeCamera = stoi(dialogBox());
		break;
	}
}

void modelMenu(int id) {
	switch (id)
	{
	case MODEL_NORMAL_V:
		scene->showNormalsV();
		break;
	case MODEL_NORMAL_F:
		scene->showNormalsF();
		break;
	case MODEL_ACTIVE:
		scene->activeModel = stoi(dialogBox());
		break;
	case MODEL_BBOX:
		scene->bbox();
	case MODEL_ROTATE:
		scene->rotate();
	}
}

void initMenu()
{
	int menuFile = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..", FILE_OPEN);

	int camFile = glutCreateMenu(camMenu);
	glutAddMenuEntry("Add", CAM_ADD);
	glutAddMenuEntry("Render", CAM_REND);
	glutAddMenuEntry("Set Active", CAM_ACTIVE);
	glutAddMenuEntry("Focus on active model", CAM_FOCUS);

	int modelFile = glutCreateMenu(modelMenu);
	glutAddMenuEntry("Set Active", MODEL_ACTIVE);
	glutAddMenuEntry("Normals per Vertex", MODEL_NORMAL_V);
	glutAddMenuEntry("Normals per Face", MODEL_NORMAL_F);
	glutAddMenuEntry("Bounding Box", MODEL_BBOX);
	glutAddMenuEntry("Rotate", MODEL_ROTATE);

	glutCreateMenu(mainMenu);
	glutAddSubMenu("File", menuFile);
	glutAddSubMenu("Camera", camFile);
	glutAddSubMenu("Model", modelFile);

	glutAddMenuEntry("Demo", MAIN_DEMO);
	glutAddMenuEntry("About", MAIN_ABOUT);
	glutAddMenuEntry("Add Primitve", MAIN_PRIM);
	glutAddMenuEntry("Set Step Size", MAIN_STEP);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------------



int my_main(int argc, char** argv)
{
	//----------------------------------------------------------------------------
	// Initialize window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("CG");
	glewExperimental = GL_TRUE;
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		/*		...*/
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));


	renderer = new Renderer(512, 512);
	scene = new Scene(renderer);
	//----------------------------------------------------------------------------
	// Initialize Callbacks

	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(catchKey);
	initMenu();


	glutMainLoop();
	delete scene;
	delete renderer;
	return 0;
}

CWinApp theApp;

using namespace std;

int main(int argc, char** argv)
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		my_main(argc, argv);
	}

	return nRetCode;
}
