#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <math.h>  
//Բ���ʺ�  
#define GL_PI 3.1415f  
//��ȡ��Ļ�Ŀ��  
GLint SCREEN_WIDTH = 0;
GLint SCREEN_HEIGHT = 0;
//���ó���Ĵ��ڴ�С  
GLint windowWidth = 400;
GLint windowHeight = 300;
//��x����ת�Ƕ�  
GLfloat xRotAngle = 0.0f;
//��y����ת�Ƕ�  
GLfloat yRotAngle = 0.0f;
//��֧�ֵĵ��С��Χ  
GLfloat sizes[2];
//��֧�ֵĵ��С����  
GLfloat step;

enum MODELS{
	SPHERE = 0,
	CONE,
	CUBE,
	TORUS,
	DODECAHEDRON,
	TEAPOT,
	OCTAHEDRON,
	TETRAHEDRON,
	ICOSAHEDRON
};
GLint iModel = SPHERE;
GLboolean bWire = GL_TRUE;
//�˵��ص�����  
void processMenu(int value){
	switch (value){
	case 1:
		iModel = SPHERE;
		break;
	case 2:
		iModel = CONE;
		break;
	case 3:
		iModel = CUBE;
		break;
	case 4:
		iModel = TORUS;
		break;
	case 5:
		iModel = DODECAHEDRON;
		break;
	case 6:
		iModel = TEAPOT;
		break;
	case 7:
		iModel = OCTAHEDRON;
		break;
	case 8:
		iModel = TETRAHEDRON;
		break;
	case 9:
		iModel = ICOSAHEDRON;
		break;
	case 10:
		bWire = GL_TRUE;
		break;
	case 11:
		bWire = GL_FALSE;
		break;
	default:
		break;
	}
	//���»���  
	glutPostRedisplay();
}
//��ʾ�ص�����  
void renderScreen(void){
	//��������ɫ����Ϊ��ɫ  
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//��ģ�建����ֵȫ������Ϊ1  
	glClearStencil(1);
	//ʹ��ģ�建����  
	glEnable(GL_STENCIL_TEST);
	//��������������Ϊ��ǰ������ɫ����ɫ�������Ȼ�������ģ�建����  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//����ǰMatrix״̬��ջ  
	glPushMatrix();
	//����ϵ��x����תxRotAngle  
	glRotatef(xRotAngle, 1.0f, 0.0f, 0.0f);
	//����ϵ��y����תyRotAngle  
	glRotatef(yRotAngle, 0.0f, 1.0f, 0.0f);
	//����ƽ������  
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);

	//��ɫ��������ϵ  
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(-9.0f, 0.0f, 0.0f);
	glVertex3f(9.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -9.0f, 0.0f);
	glVertex3f(0.0f, 9.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -9.0f);
	glVertex3f(0.0f, 0.0f, 9.0f);
	glEnd();

	glPushMatrix();
	glTranslatef(9.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidCone(0.3, 0.6, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 9.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.3, 0.6, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 9.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glutSolidCone(0.3, 0.6, 10, 10);
	glPopMatrix();

	//���û滭��ɫΪ��ɫ  
	glColor3f(0.0f, 1.0f, 0.0f);
	switch (iModel){
		//��  
	case SPHERE:
		if (bWire){
			glutWireSphere(8.0f, 20, 20);
		}
		else{
			glutSolidSphere(8.0f, 20, 20);
		}
		break;
		//׶��  
	case CONE:
		if (bWire){
			glutWireCone(4.0f, 8.0f, 20, 20);
		}
		else{
			glutSolidCone(4.0f, 8.0f, 20, 20);
		}
		break;
		//����  
	case CUBE:
		if (bWire){
			glutWireCube(8.0f);
		}
		else{
			glutSolidCube(8.0f);
		}
		break;
		//��Ȧ  
	case TORUS:
		if (bWire){
			glutWireTorus(3.0f, 6.0f, 20, 20);
		}
		else{
			glutSolidTorus(3.0f, 6.0f, 20, 20);
		}
		break;
		//ʮ�����壬Ĭ�ϰ뾶1.0  
	case DODECAHEDRON:
		glScalef(6.0f, 6.0f, 6.0f);//x,y,z����Ŵ�6��  
		if (bWire){
			glutWireDodecahedron();
		}
		else{
			glutSolidDodecahedron();
		}
		break;
		//���  
	case TEAPOT:
		if (bWire){
			glutWireTeapot(8.0f);
		}
		else{
			glutSolidTeapot(8.0f);
		}
		break;
		//�����壬Ĭ�ϰ뾶1.0  
	case OCTAHEDRON:
		glScalef(6.0f, 6.0f, 6.0f);//x,y,z����Ŵ�6��  
		if (bWire){
			glutWireOctahedron();
		}
		else{
			glutSolidOctahedron();
		}
		break;
		//�����壬Ĭ�ϰ뾶1.0  
	case TETRAHEDRON:
		glScalef(6.0f, 6.0f, 6.0f);//x,y,z����Ŵ�6��  
		if (bWire){
			glutWireTetrahedron();
		}
		else{
			glutSolidTetrahedron();
		}
		break;
		//��ʮ���壬Ĭ�ϰ뾶1.0  
	case ICOSAHEDRON:
		glScalef(6.0f, 6.0f, 6.0f);//x,y,z����Ŵ�6��  
		if (bWire){
			glutWireIcosahedron();
		}
		else{
			glutSolidIcosahedron();
		}
		break;
	default:
		break;
	}

	//�ָ�ѹ��ջ��Matrix  
	glPopMatrix();
	//����������������ָ��  
	glutSwapBuffers();
}
//����Redering State   
void setupRederingState(void){
	//����������ɫΪ��ɫ  
	glClearColor(0.0f, 0.0, 0.0, 1.0f);
	//���û滭��ɫΪ��ɫ  
	glColor3f(1.0f, 1.0f, 0.0f);
	//ʹ����Ȳ���  
	glEnable(GL_DEPTH_TEST);
	//��ȡ��֧�ֵĵ��С��Χ  
	glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
	//��ȡ��֧�ֵĵ��С����  
	glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);
	printf("point size range:%f-%f\n", sizes[0], sizes[1]);
	printf("point step:%f\n", step);
}
//���ڴ�С�仯�ص�����  
void changSize(GLint w, GLint h){
	//������  
	GLfloat ratio;
	//��������ϵΪx(-100.0f,100.0f)��y(-100.0f,100.0f)��z(-100.0f,100.0f)  
	GLfloat coordinatesize = 10.0f;
	//���ڿ��Ϊ��ֱ�ӷ���  
	if ((w == 0) || (h == 0))
		return;
	//�����ӿںʹ��ڴ�Сһ��  
	glViewport(0, 0, w, h);
	//��ͶӰ����Ӧ�����ľ������  
	glMatrixMode(GL_PROJECTION);
	//���õ�ǰָ���ľ���Ϊ��λ����  
	glLoadIdentity();
	ratio = (GLfloat)w / (GLfloat)h;
	//����ͶӰ  
	if (w<h)
		glOrtho(-coordinatesize, coordinatesize, -coordinatesize / ratio, coordinatesize / ratio, -coordinatesize, coordinatesize);
	else
		glOrtho(-coordinatesize*ratio, coordinatesize*ratio, -coordinatesize, coordinatesize, -coordinatesize, coordinatesize);
	//��ģ����ͼ�����ջӦ�����ľ������  
	glMatrixMode(GL_MODELVIEW);
	//���õ�ǰָ���ľ���Ϊ��λ����  
	glLoadIdentity();
}

//�������봦��ص�����  
void specialKey(int key, int x, int y){

	if (key == GLUT_KEY_UP){
		xRotAngle -= 5.0f;
	}
	else if (key == GLUT_KEY_DOWN){
		xRotAngle += 5.0f;
	}
	else if (key == GLUT_KEY_LEFT){
		yRotAngle -= 5.0f;
	}
	else if (key == GLUT_KEY_RIGHT){
		yRotAngle += 5.0f;
	}
	//���»���  
	glutPostRedisplay();
}

int mmain(int argc, char* argv[])
{
	int nModelMenu;
	int nWireMenu;
	int nMainMenu;
	//��ʼ��glut   
	glutInit(&argc, argv);
	//ʹ��˫����������Ȼ�������ģ�建����  
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	//��ȡϵͳ�Ŀ�����  
	SCREEN_WIDTH = glutGet(GLUT_SCREEN_WIDTH);
	//��ȡϵͳ�ĸ�����  
	SCREEN_HEIGHT = glutGet(GLUT_SCREEN_HEIGHT);
	//�������ڣ���������ΪOpenGL Glut Demo  
	glutCreateWindow("OpenGL Glut Demo");
	//���ô��ڴ�С  
	glutReshapeWindow(windowWidth, windowHeight);
	//���ھ�����ʾ  
	glutPositionWindow((SCREEN_WIDTH - windowWidth) / 2, (SCREEN_HEIGHT - windowHeight) / 2);
	//���������˵�  
	nModelMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Sphere", 1);
	glutAddMenuEntry("Cone", 2);
	glutAddMenuEntry("Cube", 3);
	glutAddMenuEntry("Torus", 4);
	glutAddMenuEntry("Dodecahedron", 5);
	glutAddMenuEntry("Teapot", 6);
	glutAddMenuEntry("Octahedron", 7);
	glutAddMenuEntry("Tetrahedron", 8);
	glutAddMenuEntry("Icosahedron", 9);
	nWireMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Wire", 10);
	glutAddMenuEntry("Solid", 11);
	nMainMenu = glutCreateMenu(processMenu);
	glutAddSubMenu("Model", nModelMenu);
	glutAddSubMenu("WireOrSolid", nWireMenu);
	//���˵��񶨵�����Ҽ���  
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	//���ڴ�С�仯ʱ�Ĵ�����  
	glutReshapeFunc(changSize);
	//������ʾ�ص�����   
	glutDisplayFunc(renderScreen);
	//���ð������봦��ص�����  
	glutSpecialFunc(specialKey);
	//����ȫ����Ⱦ����  
	setupRederingState();
	glutMainLoop();
	return 0;
}