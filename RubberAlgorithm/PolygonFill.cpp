#include "stdafx.h"
#include <iostream>
#include<gl/glut.h>
#include<algorithm>
#include<fstream>
#include<string>
#include"mydatastruct.h"//自定义所需要的数据结构，包括类和结构体等
using namespace std;
//扫描线的活化边表包含所有与该扫描线相交的边，并使用重复相关性计算来得到该扫描线与边的交点
//有序边表，对于某条特定的扫描线，表中的每一个入口包含该边的最大y值，边的x交点值和边斜率的值
static const  int screenwidth = 1000;  //自定义窗口宽度
static const int screenheight = 1000; //自定义窗口高度
int move_x, move_y; //鼠标当前坐标值，在鼠标移动动态画线时使用
bool select = false,color=false; //多边形封闭状态判断变量，当为true时即按下鼠标右键，将多边形封闭
vector<point> p; //多边形点集向量
vector<polygon> s; //多边形类向量，用来保存已经画完的多边形				   
GLfloat red = 0.0, green = 0.0, blue = 0.0;//多边形填充颜色的指定，默认为白色，通过鼠标设定即可显示指定的颜色
//鼠标事件的触发
void myMouse(int button, int state, int x, int y) //鼠标点击事件响应函数
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)//当鼠标左键被点击
	{
		point v;  //申请一个点类变量，点类为自定义类，在zl.h中定义
		v.x = x; //将点击处的点坐标，即x和y的值存入v中
		v.y = screenheight - y;
		p.push_back(v); //将点信息存入多边形点集向量p中
		glutPostRedisplay(); //重绘窗口
	}

	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) //当鼠标右键被点击
	{
		select = true;
		glutPostRedisplay();
	}
}
void myPassiveMotion(int x, int y) //鼠标移动事件响应函数
{
	move_x = x; //保存当前鼠标所在的坐标的值
	move_y = screenheight - y;
	glutPostRedisplay();//窗口重新绘制
}
bool cmp(const AETclass &a, const AETclass &b) {
	return a.x > b.x;
}
//窗口的初始化
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);//窗口的颜色设定为白色，没有调和颜色
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, screenwidth, 0.0, screenheight);//指定的二维坐标系
}
//根据传入的点集填充多边形
//对active_edge元素进行根据x的大小进行排序
//对传入的多边形进行填充颜色
void PolygonFill(polygon s)
{
	vector<point> polypoint = s.p;
	glColor3f(s.red, s.green, s.blue);//根据多变性保存的颜色，进行不同的颜色填充
	int pointnum = polypoint.size();  //每一个多边形点数.
	int i, maxy = 0, miny = 5000;
   //存放活化边表和新边表的容器向量
	vector<AETclass> AETs;	//建立活化边表
	vector<SETclass> SETs[1024];//建立新边表
	//先找到所有顶点的y坐标的上下边界即最大和最小值
	for (i = 0; i < pointnum; i++)
	{
		if (polypoint[i].y > maxy)
			maxy = polypoint[i].y;
		else if (polypoint[i].y < miny)
			miny = polypoint[i].y;
	}
	//保存每一条边的信息
	for (i = miny; i <= maxy; i++)
		for (int j = 0; j < pointnum; j++)
			if (polypoint[j].y == i)
			{
				if (polypoint[(j - 1 + pointnum) % pointnum].y > polypoint[j].y)
				{
					SETclass sclass;
					sclass.x = polypoint[j].x;
					sclass.dx = (double)(polypoint[(j - 1 + pointnum) % pointnum].x - polypoint[j].x) / (polypoint[(j - 1 + pointnum) % pointnum].y - polypoint[j].y);
					sclass.ymax = polypoint[(j - 1 + pointnum) % pointnum].y;
					SETs[i].push_back(sclass);
				}
				if (polypoint[(j + 1 + pointnum) % pointnum].y > polypoint[j].y)
				{
					SETclass sclass;
					sclass.x = polypoint[j].x;
					sclass.dx = (double)(polypoint[(j + 1 + pointnum) % pointnum].x - polypoint[j].x) / (polypoint[(j + 1 + pointnum) % pointnum].y - polypoint[j].y);;
					sclass.ymax = polypoint[(j + 1 + pointnum) % pointnum].y;
					SETs[i].push_back(sclass);
				}
			}
	//更新活性边表AET，每次遍历画一次线
	for (i = miny; i <= maxy; i++)
	{
		//更新活性边表
		int count = AETs.size();
		if (count > 0) {
			vector<AETclass>::iterator it;
			for (it = AETs.begin(); it != AETs.end();) {
				it->x = it->x + it->dx;//利用扫描线的连贯性，进行增量计算
				if (it->ymax == i) {   //删除ymax==i的结点，即减去一个像素，删除最高顶点的一个像素，防止交点计数错误
					it = AETs.erase(it);
				}
				else {
					it++;
				}
			}
		}
		//通过x的大小对交点进行排序
		sort(AETs.begin(), AETs.end(), cmp);
		vector<SETclass> vSET = SETs[i];
		//判断是否遇到端点，遇到端点则更新AET表
		int setcount = vSET.size();
		if (setcount > 0) {
			for (int seindex = 0; seindex < setcount; seindex++) {
				AETclass aet;
				aet.dx = vSET[seindex].dx;
				aet.x = vSET[seindex].x;
				aet.ymax = vSET[seindex].ymax;
				AETs.push_back(aet);
			}
		}
		//配对填充颜色
		int aetsize = AETs.size();
		if (aetsize > 1) {
			for (size_t aetindex = 0; aetindex < aetsize - 1; aetindex = aetindex + 2) {
				//根据两个交点一对一对的画线填充
				glBegin(GL_LINES); 
					glVertex2i(static_cast<int>(AETs[aetindex].x), i);
					glVertex2i(static_cast<int>(AETs[aetindex + 1].x), i);
				glEnd();
			}
		}
	}
}
//将保存多边形的容器向量，保存到文件中，保存的是每个多边形的顶点坐标值
void SavePolygon()
{
	int polygonnum = s.size();//获得多边形的数量，着一保存
	ofstream out;//开辟文件输出流
	out.open("savepolygon.txt");
	if (!out.is_open())//不存在该文件时
		cout << "error：不存在该文件，无法打开！" << endl;
	else 
		for (int i = 0; i < polygonnum; i++) 
		{
			polygon poly = s[i];
			int pointsize = poly.p.size();
			for (int j = 0; j < pointsize; j++) 
				out << poly.p[j].x << "," << poly.p[j].y << " ";//多边形点与点之间用空格间隔开
			out << "#" << poly.red << "," << poly.green << "," << poly.blue;//保存多边形的颜色值
			out << endl;//当一个多边形输入完毕，换行输入下一个多边形
		}
	out.close();
	cout << "成功保存多边形! ";
}
//根据指定字符对字符串进行分割，从而能从文件中得到所需要的坐标值，返回的是一个存放分割后的每一个字符串矢量容器
vector<string> SplitString(string srcStr, const string& delim)
{
	int nPos = 0;
	vector<string> vec;
	nPos = srcStr.find(delim.c_str());
	while (-1 != nPos)
	{
		string temp = srcStr.substr(0, nPos);
		vec.push_back(temp);
		srcStr = srcStr.substr(nPos + 1);
		nPos = srcStr.find(delim.c_str());
	}
	vec.push_back(srcStr);
	return vec;
}

//根据指定的文件从文件中读取多边形，构成多边形容器矢量，并将多边形显示出来
void GetPolygon()
{
	ifstream in("savepolygon.txt");
	vector<polygon> temp;
	string bufline;//保存从文件中读出的字符串
	if (!in.is_open())
		cout << "Error:无法打开该文件！";
	else 
		while (!in.eof())
		{
			while (getline(in, bufline))
			{
				polygon poly;
				vector<string> line = SplitString(bufline, "#");//根据#进行字符串的分割，隔离出一行中的颜色和坐标
				vector<string> coord = SplitString(line[0], " ");//根据空格得到所有每个单的坐标
				vector<string> color= SplitString(line[1], ",");
				poly.red = (float)atoi(color[0].c_str());
				poly.green = (float)atoi(color[1].c_str());
				poly.blue = (float)atoi(color[2].c_str());
				 for(int i=0;i<coord.size()-1;i++)
				 {
					 point getp;
					 getp.x=atoi(SplitString(coord[i], ",")[0].c_str());
					 getp.y = atoi(SplitString(coord[i], ",")[1].c_str());
					 poly.p.push_back(getp);
				 }
				 temp.push_back(poly);
			}
		}
	//从文件中读出多边形，然后进行赋值显示
	for (int i = 0; i < temp.size(); i++)
		s.push_back(temp[i]);
	in.close();
	cout << "成功读取多边形文件！";
}
//多边形填充算法的核心功能实现
void lineSegment()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//glColor3f(red, green, blue);   //设定颜色，既是线段颜色也是填充色---红色
	glColor3f(red, green, blue);
	int i, j;                 //两个循环控制变量，在下面的向量循环和数组循环中将被多次调用
	//看多边形类向量是否为空，即判断除了当前正在画的多边形是否还有曾经已经画好的多边形
	if (!s.empty())            
	{
		for (i = 0; i < s.size(); i++)   //对多边形类向量循环，该向量中的每个元素代表一个多边形
		{
			GLfloat a=s[i].red,b=s[i].green,c=s[i].blue;
			glColor3f(a, b, c);
			int k = s[i].p.size();  //将一个多边形的点的个数
			for (j = 0; j < k; j++) //画多边形
			{
				glBegin(GL_LINES); //将当前的点与后一个点连线
					glVertex2i(s[i].p[j].x, s[i].p[j].y);
					glVertex2i(s[i].p[(j + 1) % k].x, s[i].p[(j + 1) % k].y);//，通过取模操作来避免越界问题，该思路来源于循环队列.
				glEnd();
			}
			PolygonFill(s[i]);
		}
	}
	i = 0;
	j = p.size() - 1;
	//将已经确定的点连接起来
	while (i < j)           //循环画图，将当前正在画的多边形
	{
		glBegin(GL_LINES);
			glVertex2i(p[i].x, p[i].y);
			glVertex2i(p[i + 1].x, p[i + 1].y);
		glEnd();
		i++;
	}
	//将确定的最后一个点与当前鼠标所在位置连线，即动态画线 如同橡皮筋
	if (!p.empty())
	{
		glBegin(GL_LINES);
			glVertex2i(p[j].x, p[j].y);
			glVertex2i(move_x, move_y);
		glEnd();
	}
	if (select) //判断右键是否被点下
	{
		select = false; //将状态值置为假
		if (!p.empty())
		{
			glBegin(GL_LINES); //将多边形封闭
				glVertex2i(p[j].x, p[j].y);
				glVertex2i(p[0].x, p[0].y);
			glEnd();
			polygon sq;//当按下了鼠标右键才会将完整的该多边形的所有点集作为一个多边形放入多边形类中
			//将封闭了的多边形保存到多边形类中
			for (i = 0; i < p.size(); i++)
				sq.p.push_back(p[i]);
			sq.red = red;
			sq.green = green;
			sq.blue = blue;
			s.push_back(sq); //将绘成的多边形存入多边形类向量中			
		}
		p.clear();//p每次存完一个完整的多边形以后就进行清空，以备后面的多边形点集存放使用
	}
	glFlush();
}
void clearpolygon() {
	s.pop_back();
	cout << "Last polygon has been cleared!" << endl;
	glFlush();//刷新缓存
}
void clearpolygonAll() {
	s.clear();
	cout << "All polygons have been cleared!" << endl;
	glFlush();//刷新缓存
}
//对窗口的大小改变以后的图像的重新绘制
void reshapeFcn(GLint newWidth,GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, GLfloat (newWidth), 0.0,GLfloat (newHeight));//指定的二维坐标系
	lineSegment();
	glFlush();
}
//菜单交互的菜单设计定义
void mainMenu(GLint option)
{
	switch (option)
	{
		case 2:
			clearpolygon();//清理重置窗口
			break;
		case 3:
			clearpolygonAll();//清理重置窗口
			break;
		case 4:
			SavePolygon();//文件存储
			break;
		case 5:
			GetPolygon();//文件读取
			break;
	}
	glutPostRedisplay();
}
void renderSubMenu(GLint option)
{
	switch (option)
	{
	case 1:;
		break;
	case 2:;
		break;
	}
	glutPostRedisplay();
}
//根据菜单传回的参数，调用不同的颜色值
void colorSubMenu(GLint option)
{
	switch (option)
	{
	case 1:
		red = 0.0; green = 0.0;blue=1.0;//蓝色
		break;
	case 2:
		red = 0.0; green = 1.0; blue = 0.0;//绿色
		break;
	case 3:
		red = 1.0; green = 0.0; blue = 0.0;//红色
		break;
	case 4:
		red = 1.0; green = 1.0; blue = 0.0;//黄色
		break;
	case 5:
		red = 1.0; green = 0.0; blue = 1.0;//品红色
		break;
	case 6:
		red = 0.0; green = 1.0; blue = 1.0;//青色
		break;
	}
	glutPostRedisplay();
}
int main(int argc, char** argv)
{
	GLint subMenu1, subMenu2;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50, 100);
	glutInitWindowSize(screenwidth, screenheight);
	glutCreateWindow("多边形扫描填充算法");
	init();
	glutMouseFunc(myMouse); //鼠标点击消息监控，即监控鼠标是否被点击，若被点击就调用myMouse函数
	glutPassiveMotionFunc(myPassiveMotion); //鼠标移动消息监控，即监控鼠标是否移动，若移动就调用myPassiveMotion函数
	glLineWidth(1.0);
	glutDisplayFunc(lineSegment);
	//菜单交互的设计
	subMenu1 = glutCreateMenu(renderSubMenu);
		glutAddMenuEntry("Scan-Line  Fill", 1);//扫描线填充算法
		glutAddMenuEntry("Boundary Fill", 2);//边界填充算法
		glutAddMenuEntry("Flood Fill", 3);//泛滥填充算法
	subMenu2=glutCreateMenu(colorSubMenu);
		glutAddMenuEntry("Blue", 1);
		glutAddMenuEntry("Green", 2);
		glutAddMenuEntry("Red", 3);
		glutAddMenuEntry("Yellow", 4);
		glutAddMenuEntry("Magenta", 5);
		glutAddMenuEntry("Verdant", 6);
	glutCreateMenu(mainMenu);
		glutAddMenuEntry("多边形填充算法菜单", 0);
		glutAddSubMenu("填充模式", subMenu1);
		glutAddSubMenu("填充颜色",subMenu2);
		glutAddMenuEntry("撤销上一个", 2);
		glutAddMenuEntry("清理重置", 3);
		glutAddMenuEntry("文件存储", 4);
		glutAddMenuEntry("文件读出", 5);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);//将菜单与鼠标的右键绑定起来
	glutReshapeFunc(reshapeFcn);
	glutMainLoop();
	return 0;
}
