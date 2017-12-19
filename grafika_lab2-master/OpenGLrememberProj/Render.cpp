#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

double i = 0;
double j = 0;

bool stop_click = false;


int tick = GetTickCount();
bool textureMode = true;
bool lightMode = true;
bool alpha = false;
//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'S')
	{
		if (stop_click)
			stop_click = false;
		else
			stop_click = true;
	}



	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == 'P' && !alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		alpha = true;
	}

	else if (key == 'P' && alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glDisable(GL_BLEND);
		alpha = false;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

void normal(double a1[], double b1[], double c1[], double vn[])
{
	double a[] = { a1[0] - b1[0],a1[1] - b1[1],a1[2] - b1[2] };
	double b[] = { c1[0] - b1[0],c1[1] - b1[1],c1[2] - b1[2] };

	vn[0] = a[1] * b[2] - b[1] * a[2];
	vn[1] = -a[0] * b[2] + b[0] * a[2];
	vn[2] = a[0] * b[1] - b[0] * a[1];

	double length = sqrt(pow(vn[0], 2) + pow(vn[1], 2) + pow(vn[2], 2));

	vn[0] /= length;
	vn[1] /= length;
	vn[2] /= length;
}
void Render(OpenGL *ogl)
{       	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  

	//cube(100);

	double qua11[] = { 6, 6, 4 };
	double qua12[] = { 6, 6, 8 };
	double qua13[] = { 10, 6, 8 };
	double qua14[] = { 10, 6, 4 };
	double qua21[] = { 6, 10, 4 };
	double qua22[] = { 6, 10, 8 };
	double qua23[] = { 10, 10, 8 };
	double qua24[] = { 10, 10, 4 };

	double tr11[] = { 5, 6, 4 };
	double tr12[] = { 2, 8, 6 };
	double tr13[] = { 5, 6, 8 };
	double tr14[] = { 5, 10, 8 };
	double tr15[] = { 5, 10, 4 };

	double tr21[] = { 6, 6, 9 };
	double tr22[] = { 8, 8, 12 };
	double tr23[] = { 10, 6, 9 };
	double tr24[] = { 10, 10, 9 };
	double tr25[] = { 6, 10, 9 };

	double tr31[] = { 11, 6, 4 };
	double tr32[] = { 14, 8, 6 };
	double tr33[] = { 11, 6, 8 };
	double tr34[] = { 11, 10, 8 };
	double tr35[] = { 11, 10, 4 };

	double tr41[] = { 10, 6, 3 };
	double tr42[] = { 8, 8, 0 };
	double tr43[] = { 6, 6, 3 };
	double tr44[] = { 6, 10, 3 };
	double tr45[] = { 10, 10, 3 };

	double tr51[] = { 6, 5, 4 };
	double tr52[] = { 8, 2, 6 };
	double tr53[] = { 6, 5, 8 };
	double tr54[] = { 10, 5, 8 };
	double tr55[] = { 10, 5, 4 };


	double tr61[] = { 6, 11, 4 };
	double tr62[] = { 8, 14, 6 };
	double tr63[] = { 6, 11, 8 };
	double tr64[] = { 10, 11, 8 };
	double tr65[] = { 10, 11, 4 };


	double qua31[] = { 4, 4, -7 };
	double qua32[] = { 4, 12, -7 };
	double qua33[] = { 12, 12, -7 };
	double qua34[] = { 12, 4, -7 };

	double qua41[] = { 4, 4, -14 };
	double qua42[] = { 4, 12, -14 };
	double qua43[] = { 12, 12, -14 };
	double qua44[] = { 12, 4, -14 };

	double c0[] = { 8, 8, -6 };
	double c1[] = { 8, 2, -5 };
	double c2[] = { 2, 8, -5 };
	double c3[] = { 8, 14, -5 };
	double c4[] = { 14, 8, -5 };

	double v1[] = { 0, 0, 2 };
	double v2[] = { 0, 16, 2 };
	double v3[] = { 16, 16, 2 };
	double v4[] = { 16, 0, 2 };

	double c21[] = { 8, 4, -7 };
	double c22[] = { 4, 8, -7 };
	double c23[] = { 8, 12, -7 };
	double c24[] = { 12, 8, -7 };


	double vn[] = { 0,0,0 };//вектор нормали

	glColor3f(1.0f, 0.839f, 0.0f);


	glBegin(GL_QUADS); // постамент

	normal(qua31, qua32, qua33, vn);
	glNormal3dv(vn);
	glVertex3dv(qua31);
	glVertex3dv(qua32);
	glVertex3dv(qua33);
	glVertex3dv(qua34);

	normal(qua43, qua42, qua41, vn);
	glNormal3dv(vn);
	glVertex3dv(qua41);
	glVertex3dv(qua42);
	glVertex3dv(qua43);
	glVertex3dv(qua44);

	normal(qua42, qua32, qua31, vn);
	glNormal3dv(vn);
	glVertex3dv(qua31);
	glVertex3dv(qua32);
	glVertex3dv(qua42);
	glVertex3dv(qua41);

	normal(qua31, qua34, qua44, vn);
	glNormal3dv(vn);
	glVertex3dv(qua31);
	glVertex3dv(qua34);
	glVertex3dv(qua44);
	glVertex3dv(qua41);

	normal(qua22, qua23, qua13, vn);
	glNormal3dv(vn);
	glVertex3dv(qua32);
	glVertex3dv(qua33);
	glVertex3dv(qua43);
	glVertex3dv(qua42);

	normal(qua34, qua33, qua43, vn);
	glNormal3dv(vn);
	glVertex3dv(qua34);
	glVertex3dv(qua33);
	glVertex3dv(qua43);
	glVertex3dv(qua44);

	glEnd();

	glBegin(GL_TRIANGLES);  //треугольники руки

	normal(v1, c0, c1, vn);
	glNormal3dv(vn);
	glVertex3dv(v1);
	glVertex3dv(c0);
	glVertex3dv(c1);

	normal(v1, c0, c2, vn);
	glNormal3dv(vn);
	glVertex3dv(v1);
	glVertex3dv(c0);
	glVertex3dv(c2);

	normal(v2, c0, c2, vn);
	glNormal3dv(vn);
	glVertex3dv(v2);
	glVertex3dv(c0);
	glVertex3dv(c2);

	normal(v2, c0, c3, vn);
	glNormal3dv(vn);
	glVertex3dv(v2);
	glVertex3dv(c0);
	glVertex3dv(c3);

	normal(v3, c0, c3, vn);
	glNormal3dv(vn);
	glVertex3dv(v3);
	glVertex3dv(c0);
	glVertex3dv(c3);

	normal(v3, c0, c4, vn);
	glNormal3dv(vn);
	glVertex3dv(v3);
	glVertex3dv(c0);
	glVertex3dv(c4);

	normal(v4, c0, c4, vn);
	glNormal3dv(vn);
	glVertex3dv(v4);
	glVertex3dv(c0);
	glVertex3dv(c4);

	normal(v4, c0, c1, vn);
	glNormal3dv(vn);
	glVertex3dv(v4);
	glVertex3dv(c0);
	glVertex3dv(c1);

	glEnd();

	glBegin(GL_QUADS); //квадраты руки

	normal(c21, qua31, v1, vn);
	glNormal3dv(vn);
	glVertex3dv(v1);
	glVertex3dv(qua31);
	glVertex3dv(c21);
	glVertex3dv(c1);

	normal(v1, qua31, c22, vn);
	glNormal3dv(vn);
	glVertex3dv(v1);
	glVertex3dv(qua31);
	glVertex3dv(c22);
	glVertex3dv(c2);

	normal(c21, qua31, v1, vn);
	glNormal3dv(vn);
	glVertex3dv(v2);
	glVertex3dv(qua32);
	glVertex3dv(c22);
	glVertex3dv(c2);

	normal(v2, qua31, c23, vn);
	glNormal3dv(vn);
	glVertex3dv(v2);
	glVertex3dv(qua32);
	glVertex3dv(c23);
	glVertex3dv(c3);

	normal(c21, qua31, v1, vn);
	glNormal3dv(vn);
	glVertex3dv(v3);
	glVertex3dv(qua33);
	glVertex3dv(c23);
	glVertex3dv(c3);

	normal(v3, qua31, c23, vn);
	glNormal3dv(vn);
	glVertex3dv(v3);
	glVertex3dv(qua33);
	glVertex3dv(c24);
	glVertex3dv(c4);

	normal(c21, qua31, v1, vn);
	glNormal3dv(vn);
	glVertex3dv(v4);
	glVertex3dv(qua34);
	glVertex3dv(c24);
	glVertex3dv(c4);

	normal(v3, qua31, c23, vn);
	glNormal3dv(vn);
	glVertex3dv(v4);
	glVertex3dv(qua34);
	glVertex3dv(c21);
	glVertex3dv(c1);



	glEnd();




	if (!stop_click)
	{
		if (GetTickCount() - tick > 1)
		{

			glRotated(i, 1, 1, 1);
			i = i + 0.5;


			tick = GetTickCount();
		}
	}

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.839f, 0.0f);

	normal(qua11, qua12, qua13, vn);
	glNormal3dv(vn);
	glVertex3dv(qua11); //Cube begins
	glVertex3dv(qua12);
	glVertex3dv(qua13);
	glVertex3dv(qua14);

	normal(qua22, qua12, qua11, vn);
	glNormal3dv(vn);
	glVertex3dv(qua11);
	glVertex3dv(qua12);
	glVertex3dv(qua22);
	glVertex3dv(qua21);

	normal(qua11, qua14, qua24, vn);
	glNormal3dv(vn);
	glVertex3dv(qua11);
	glVertex3dv(qua14);
	glVertex3dv(qua24);
	glVertex3dv(qua21);

	normal(qua22, qua21, qua23, vn);
	glNormal3dv(vn);
	glVertex3dv(qua21);
	glVertex3dv(qua22);
	glVertex3dv(qua23);
	glVertex3dv(qua24);

	normal(qua23, qua14, qua13, vn);
	glNormal3dv(vn);
	glVertex3dv(qua14);
	glVertex3dv(qua13);
	glVertex3dv(qua23);
	glVertex3dv(qua24);

	normal(qua22, qua23, qua13, vn);
	glNormal3dv(vn);
	glVertex3dv(qua12);
	glVertex3dv(qua13);
	glVertex3dv(qua23);
	glVertex3dv(qua22);	//Cube ends


	glEnd();



	//настройка материала
	GLfloat ambient[] = { 0.4, 0.4, 0.2, 1. };
	GLfloat diffuse[] = { 0.1, 0.25, 0.3, 1. };
	GLfloat specular[] = { 0.5, 0.4, 0.8, 1. };
	GLfloat shiny = 0.2f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, shiny);


	//first pyramide begins
	glBegin(GL_TRIANGLES);

	glColor3f(0.929f, 0.894f, 0.867f);

	normal(tr11, tr12, tr13, vn);
	glNormal3dv(vn);
	glVertex3dv(tr11);
	glVertex3dv(tr12);
	glVertex3dv(tr13);

	normal(tr15, tr12, tr11, vn);
	glNormal3dv(vn);
	glVertex3dv(tr11);
	glVertex3dv(tr12);
	glVertex3dv(tr15);

	normal(tr13, tr12, tr14, vn);
	glNormal3dv(vn);
	glVertex3dv(tr13);
	glVertex3dv(tr12);
	glVertex3dv(tr14);

	normal(tr14, tr12, tr15, vn);
	glNormal3dv(vn);
	glVertex3dv(tr14);
	glVertex3dv(tr12);
	glVertex3dv(tr15);

	glEnd();

	glBegin(GL_QUADS);
	normal(tr11, tr13, tr15, vn);
	glNormal3dv(vn);
	glVertex3dv(tr11);	//first pyramide
	glVertex3dv(tr13);
	glVertex3dv(tr14);
	glVertex3dv(tr15);
	glEnd();

	//first pyramide ends


	//second pyramide begins
	glBegin(GL_TRIANGLES);

	normal(tr21, tr22, tr23, vn);
	glNormal3dv(vn);
	glVertex3dv(tr21);
	glVertex3dv(tr22);
	glVertex3dv(tr23);

	normal(tr25, tr22, tr21, vn);
	glNormal3dv(vn);
	glVertex3dv(tr21);
	glVertex3dv(tr22);
	glVertex3dv(tr25);

	normal(tr23, tr22, tr24, vn);
	glNormal3dv(vn);
	glVertex3dv(tr23);
	glVertex3dv(tr22);
	glVertex3dv(tr24);

	normal(tr24, tr22, tr25, vn);
	glNormal3dv(vn);
	glVertex3dv(tr24);
	glVertex3dv(tr22);
	glVertex3dv(tr25);

	glEnd();

	glBegin(GL_QUADS);
	normal(tr25, tr21, tr23, vn);
	glNormal3dv(vn);
	glVertex3dv(tr21);	//second pyramide
	glVertex3dv(tr23);
	glVertex3dv(tr24);
	glVertex3dv(tr25);
	glEnd();

	//second pyramide ends

	//third pyramide begins
	glBegin(GL_TRIANGLES);

	normal(tr32, tr31, tr33, vn);
	glNormal3dv(vn);
	glVertex3dv(tr31);
	glVertex3dv(tr32);
	glVertex3dv(tr33);

	normal(tr35, tr31, tr32, vn);
	glNormal3dv(vn);
	glVertex3dv(tr31);
	glVertex3dv(tr32);
	glVertex3dv(tr35);

	normal(tr33, tr34, tr32, vn);
	glNormal3dv(vn);
	glVertex3dv(tr33);
	glVertex3dv(tr32);
	glVertex3dv(tr34);

	normal(tr35, tr32, tr34, vn);
	glNormal3dv(vn);
	glVertex3dv(tr32);
	glVertex3dv(tr34);
	glVertex3dv(tr35);

	glEnd();


	glBegin(GL_QUADS);
	normal(tr34, tr33, tr35, vn);
	glNormal3dv(vn);
	glVertex3dv(tr31);	//third pyramide
	glVertex3dv(tr33);
	glVertex3dv(tr34);
	glVertex3dv(tr35);
	glEnd();

	//third pyramide ends

	//fourd pyramide begins
	glBegin(GL_TRIANGLES);

	normal(tr41, tr42, tr43, vn);
	glNormal3dv(vn);
	glVertex3dv(tr41);
	glVertex3dv(tr42);
	glVertex3dv(tr43);

	normal(tr45, tr42, tr41, vn);
	glNormal3dv(vn);
	glVertex3dv(tr41);
	glVertex3dv(tr42);
	glVertex3dv(tr45);

	normal(tr42, tr44, tr43, vn);
	glNormal3dv(vn);
	glVertex3dv(tr43);
	glVertex3dv(tr42);
	glVertex3dv(tr44);

	normal(tr44, tr42, tr45, vn);
	glNormal3dv(vn);
	glVertex3dv(tr42);
	glVertex3dv(tr44);
	glVertex3dv(tr45);

	glEnd();


	glBegin(GL_QUADS);
	normal(tr41, tr43, tr44, vn);
	glNormal3dv(vn);
	glVertex3dv(tr41);	//fourd pyramide
	glVertex3dv(tr43);
	glVertex3dv(tr44);
	glVertex3dv(tr45);
	glEnd();

	//fourd pyramide ends

	//fifth pyramide begins
	glBegin(GL_TRIANGLES);

	normal(tr52, tr51, tr53, vn);
	glNormal3dv(vn);
	glVertex3dv(tr51);
	glVertex3dv(tr52);
	glVertex3dv(tr53);

	normal(tr52, tr55, tr51, vn);
	glNormal3dv(vn);
	glVertex3dv(tr51);
	glVertex3dv(tr52);
	glVertex3dv(tr55);

	normal(tr52, tr53, tr54, vn);
	glNormal3dv(vn);
	glVertex3dv(tr53);
	glVertex3dv(tr52);
	glVertex3dv(tr54);

	normal(tr55, tr52, tr54, vn);
	glNormal3dv(vn);
	glVertex3dv(tr52);
	glVertex3dv(tr54);
	glVertex3dv(tr55);

	glEnd();

	glBegin(GL_QUADS);
	normal(tr55, tr53, tr51, vn);
	glNormal3dv(vn);
	glVertex3dv(tr51);	//fifth pyramide
	glVertex3dv(tr53);
	glVertex3dv(tr54);
	glVertex3dv(tr55);
	glEnd();

	//fifth pyramide ends

	//sixth pyramide begins
	glBegin(GL_TRIANGLES);

	normal(tr61, tr62, tr63, vn);
	glNormal3dv(vn);
	glVertex3dv(tr61);
	glVertex3dv(tr62);
	glVertex3dv(tr63);

	normal(tr65, tr62, tr61, vn);
	glNormal3dv(vn);
	glVertex3dv(tr61);
	glVertex3dv(tr62);
	glVertex3dv(tr65);

	normal(tr63, tr62, tr64, vn);
	glNormal3dv(vn);
	glVertex3dv(tr63);
	glVertex3dv(tr62);
	glVertex3dv(tr64);

	normal(tr65, tr64, tr62, vn);
	glNormal3dv(vn);
	glVertex3dv(tr62);
	glVertex3dv(tr64);
	glVertex3dv(tr65);

	glEnd();

	glBegin(GL_QUADS);
	normal(tr61, tr63, tr65, vn);
	glNormal3dv(vn);
	glVertex3dv(tr61);	//sixth pyramide
	glVertex3dv(tr63);
	glVertex3dv(tr64);
	glVertex3dv(tr65);
	glEnd();

	//sixth pyramide ends



	//Начало рисования квадратика станкина
	//	double A[2] = { -4, -4 };
	//	double B[2] = { 4, -4 };
	//	double C[2] = { 4, 4 };
	//	double D[2] = { -4, 4 };



	//	glBegin(GL_QUADS);

	//	glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	////glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);

	//glEnd();
	//конец рисования квадратика станкина


	//текст сообщения вверху слева, если надоест - закоментировать, или заменить =)
	char c[250];  //максимальная длина сообщения
	sprintf_s(c, "(T)Текстуры - %d\n(L)Свет - %d\n\nУправление светом:\n"
		"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
		"R - установить камеру и свет в начальное положение\n"
		"F - переместить свет в точку камеры", textureMode, lightMode);
	ogl->message = std::string(c);




}   //конец тела функции

