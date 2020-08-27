#include "stdafx.h"
#include <iostream>
#include<gl/glut.h>
#include<algorithm>
#include<fstream>
#include<string>
#include"mydatastruct.h"//�Զ�������Ҫ�����ݽṹ��������ͽṹ���
using namespace std;
//ɨ���ߵĻ�߱�����������ɨ�����ཻ�ıߣ���ʹ���ظ�����Լ������õ���ɨ������ߵĽ���
//����߱�����ĳ���ض���ɨ���ߣ����е�ÿһ����ڰ����ñߵ����yֵ���ߵ�x����ֵ�ͱ�б�ʵ�ֵ
static const  int screenwidth = 1000;  //�Զ��崰�ڿ��
static const int screenheight = 1000; //�Զ��崰�ڸ߶�
int move_x, move_y; //��굱ǰ����ֵ��������ƶ���̬����ʱʹ��
bool select = false,color=false; //����η��״̬�жϱ�������Ϊtrueʱ����������Ҽ���������η��
vector<point> p; //����ε㼯����
vector<polygon> s; //����������������������Ѿ�����Ķ����				   
GLfloat red = 0.0, green = 0.0, blue = 0.0;//����������ɫ��ָ����Ĭ��Ϊ��ɫ��ͨ������趨������ʾָ������ɫ
//����¼��Ĵ���
void myMouse(int button, int state, int x, int y) //������¼���Ӧ����
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)//�������������
	{
		point v;  //����һ���������������Ϊ�Զ����࣬��zl.h�ж���
		v.x = x; //��������ĵ����꣬��x��y��ֵ����v��
		v.y = screenheight - y;
		p.push_back(v); //������Ϣ�������ε㼯����p��
		glutPostRedisplay(); //�ػ洰��
	}

	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) //������Ҽ������
	{
		select = true;
		glutPostRedisplay();
	}
}
void myPassiveMotion(int x, int y) //����ƶ��¼���Ӧ����
{
	move_x = x; //���浱ǰ������ڵ������ֵ
	move_y = screenheight - y;
	glutPostRedisplay();//�������»���
}
bool cmp(const AETclass &a, const AETclass &b) {
	return a.x > b.x;
}
//���ڵĳ�ʼ��
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);//���ڵ���ɫ�趨Ϊ��ɫ��û�е�����ɫ
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, screenwidth, 0.0, screenheight);//ָ���Ķ�ά����ϵ
}
//���ݴ���ĵ㼯�������
//��active_edgeԪ�ؽ��и���x�Ĵ�С��������
//�Դ���Ķ���ν��������ɫ
void PolygonFill(polygon s)
{
	vector<point> polypoint = s.p;
	glColor3f(s.red, s.green, s.blue);//���ݶ���Ա������ɫ�����в�ͬ����ɫ���
	int pointnum = polypoint.size();  //ÿһ������ε���.
	int i, maxy = 0, miny = 5000;
   //��Ż�߱���±߱����������
	vector<AETclass> AETs;	//������߱�
	vector<SETclass> SETs[1024];//�����±߱�
	//���ҵ����ж����y��������±߽缴������Сֵ
	for (i = 0; i < pointnum; i++)
	{
		if (polypoint[i].y > maxy)
			maxy = polypoint[i].y;
		else if (polypoint[i].y < miny)
			miny = polypoint[i].y;
	}
	//����ÿһ���ߵ���Ϣ
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
	//���»��Ա߱�AET��ÿ�α�����һ����
	for (i = miny; i <= maxy; i++)
	{
		//���»��Ա߱�
		int count = AETs.size();
		if (count > 0) {
			vector<AETclass>::iterator it;
			for (it = AETs.begin(); it != AETs.end();) {
				it->x = it->x + it->dx;//����ɨ���ߵ������ԣ�������������
				if (it->ymax == i) {   //ɾ��ymax==i�Ľ�㣬����ȥһ�����أ�ɾ����߶����һ�����أ���ֹ�����������
					it = AETs.erase(it);
				}
				else {
					it++;
				}
			}
		}
		//ͨ��x�Ĵ�С�Խ����������
		sort(AETs.begin(), AETs.end(), cmp);
		vector<SETclass> vSET = SETs[i];
		//�ж��Ƿ������˵㣬�����˵������AET��
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
		//��������ɫ
		int aetsize = AETs.size();
		if (aetsize > 1) {
			for (size_t aetindex = 0; aetindex < aetsize - 1; aetindex = aetindex + 2) {
				//������������һ��һ�ԵĻ������
				glBegin(GL_LINES); 
					glVertex2i(static_cast<int>(AETs[aetindex].x), i);
					glVertex2i(static_cast<int>(AETs[aetindex + 1].x), i);
				glEnd();
			}
		}
	}
}
//���������ε��������������浽�ļ��У��������ÿ������εĶ�������ֵ
void SavePolygon()
{
	int polygonnum = s.size();//��ö���ε���������һ����
	ofstream out;//�����ļ������
	out.open("savepolygon.txt");
	if (!out.is_open())//�����ڸ��ļ�ʱ
		cout << "error�������ڸ��ļ����޷��򿪣�" << endl;
	else 
		for (int i = 0; i < polygonnum; i++) 
		{
			polygon poly = s[i];
			int pointsize = poly.p.size();
			for (int j = 0; j < pointsize; j++) 
				out << poly.p[j].x << "," << poly.p[j].y << " ";//����ε����֮���ÿո�����
			out << "#" << poly.red << "," << poly.green << "," << poly.blue;//�������ε���ɫֵ
			out << endl;//��һ�������������ϣ�����������һ�������
		}
	out.close();
	cout << "�ɹ���������! ";
}
//����ָ���ַ����ַ������зָ�Ӷ��ܴ��ļ��еõ�����Ҫ������ֵ�����ص���һ����ŷָ���ÿһ���ַ���ʸ������
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

//����ָ�����ļ����ļ��ж�ȡ����Σ����ɶ��������ʸ���������������ʾ����
void GetPolygon()
{
	ifstream in("savepolygon.txt");
	vector<polygon> temp;
	string bufline;//������ļ��ж������ַ���
	if (!in.is_open())
		cout << "Error:�޷��򿪸��ļ���";
	else 
		while (!in.eof())
		{
			while (getline(in, bufline))
			{
				polygon poly;
				vector<string> line = SplitString(bufline, "#");//����#�����ַ����ķָ�����һ���е���ɫ������
				vector<string> coord = SplitString(line[0], " ");//���ݿո�õ�����ÿ����������
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
	//���ļ��ж�������Σ�Ȼ����и�ֵ��ʾ
	for (int i = 0; i < temp.size(); i++)
		s.push_back(temp[i]);
	in.close();
	cout << "�ɹ���ȡ������ļ���";
}
//���������㷨�ĺ��Ĺ���ʵ��
void lineSegment()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//glColor3f(red, green, blue);   //�趨��ɫ�������߶���ɫҲ�����ɫ---��ɫ
	glColor3f(red, green, blue);
	int i, j;                 //����ѭ�����Ʊ����������������ѭ��������ѭ���н�����ε���
	//��������������Ƿ�Ϊ�գ����жϳ��˵�ǰ���ڻ��Ķ�����Ƿ��������Ѿ����õĶ����
	if (!s.empty())            
	{
		for (i = 0; i < s.size(); i++)   //�Զ����������ѭ�����������е�ÿ��Ԫ�ش���һ�������
		{
			GLfloat a=s[i].red,b=s[i].green,c=s[i].blue;
			glColor3f(a, b, c);
			int k = s[i].p.size();  //��һ������εĵ�ĸ���
			for (j = 0; j < k; j++) //�������
			{
				glBegin(GL_LINES); //����ǰ�ĵ����һ��������
					glVertex2i(s[i].p[j].x, s[i].p[j].y);
					glVertex2i(s[i].p[(j + 1) % k].x, s[i].p[(j + 1) % k].y);//��ͨ��ȡģ����������Խ�����⣬��˼·��Դ��ѭ������.
				glEnd();
			}
			PolygonFill(s[i]);
		}
	}
	i = 0;
	j = p.size() - 1;
	//���Ѿ�ȷ���ĵ���������
	while (i < j)           //ѭ����ͼ������ǰ���ڻ��Ķ����
	{
		glBegin(GL_LINES);
			glVertex2i(p[i].x, p[i].y);
			glVertex2i(p[i + 1].x, p[i + 1].y);
		glEnd();
		i++;
	}
	//��ȷ�������һ�����뵱ǰ�������λ�����ߣ�����̬���� ��ͬ��Ƥ��
	if (!p.empty())
	{
		glBegin(GL_LINES);
			glVertex2i(p[j].x, p[j].y);
			glVertex2i(move_x, move_y);
		glEnd();
	}
	if (select) //�ж��Ҽ��Ƿ񱻵���
	{
		select = false; //��״ֵ̬��Ϊ��
		if (!p.empty())
		{
			glBegin(GL_LINES); //������η��
				glVertex2i(p[j].x, p[j].y);
				glVertex2i(p[0].x, p[0].y);
			glEnd();
			polygon sq;//������������Ҽ��ŻὫ�����ĸö���ε����е㼯��Ϊһ������η�����������
			//������˵Ķ���α��浽���������
			for (i = 0; i < p.size(); i++)
				sq.p.push_back(p[i]);
			sq.red = red;
			sq.green = green;
			sq.blue = blue;
			s.push_back(sq); //����ɵĶ���δ���������������			
		}
		p.clear();//pÿ�δ���һ�������Ķ�����Ժ�ͽ�����գ��Ա�����Ķ���ε㼯���ʹ��
	}
	glFlush();
}
void clearpolygon() {
	s.pop_back();
	cout << "Last polygon has been cleared!" << endl;
	glFlush();//ˢ�»���
}
void clearpolygonAll() {
	s.clear();
	cout << "All polygons have been cleared!" << endl;
	glFlush();//ˢ�»���
}
//�Դ��ڵĴ�С�ı��Ժ��ͼ������»���
void reshapeFcn(GLint newWidth,GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, GLfloat (newWidth), 0.0,GLfloat (newHeight));//ָ���Ķ�ά����ϵ
	lineSegment();
	glFlush();
}
//�˵������Ĳ˵���ƶ���
void mainMenu(GLint option)
{
	switch (option)
	{
		case 2:
			clearpolygon();//�������ô���
			break;
		case 3:
			clearpolygonAll();//�������ô���
			break;
		case 4:
			SavePolygon();//�ļ��洢
			break;
		case 5:
			GetPolygon();//�ļ���ȡ
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
//���ݲ˵����صĲ��������ò�ͬ����ɫֵ
void colorSubMenu(GLint option)
{
	switch (option)
	{
	case 1:
		red = 0.0; green = 0.0;blue=1.0;//��ɫ
		break;
	case 2:
		red = 0.0; green = 1.0; blue = 0.0;//��ɫ
		break;
	case 3:
		red = 1.0; green = 0.0; blue = 0.0;//��ɫ
		break;
	case 4:
		red = 1.0; green = 1.0; blue = 0.0;//��ɫ
		break;
	case 5:
		red = 1.0; green = 0.0; blue = 1.0;//Ʒ��ɫ
		break;
	case 6:
		red = 0.0; green = 1.0; blue = 1.0;//��ɫ
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
	glutCreateWindow("�����ɨ������㷨");
	init();
	glutMouseFunc(myMouse); //�������Ϣ��أ����������Ƿ񱻵������������͵���myMouse����
	glutPassiveMotionFunc(myPassiveMotion); //����ƶ���Ϣ��أ����������Ƿ��ƶ������ƶ��͵���myPassiveMotion����
	glLineWidth(1.0);
	glutDisplayFunc(lineSegment);
	//�˵����������
	subMenu1 = glutCreateMenu(renderSubMenu);
		glutAddMenuEntry("Scan-Line  Fill", 1);//ɨ��������㷨
		glutAddMenuEntry("Boundary Fill", 2);//�߽�����㷨
		glutAddMenuEntry("Flood Fill", 3);//��������㷨
	subMenu2=glutCreateMenu(colorSubMenu);
		glutAddMenuEntry("Blue", 1);
		glutAddMenuEntry("Green", 2);
		glutAddMenuEntry("Red", 3);
		glutAddMenuEntry("Yellow", 4);
		glutAddMenuEntry("Magenta", 5);
		glutAddMenuEntry("Verdant", 6);
	glutCreateMenu(mainMenu);
		glutAddMenuEntry("���������㷨�˵�", 0);
		glutAddSubMenu("���ģʽ", subMenu1);
		glutAddSubMenu("�����ɫ",subMenu2);
		glutAddMenuEntry("������һ��", 2);
		glutAddMenuEntry("��������", 3);
		glutAddMenuEntry("�ļ��洢", 4);
		glutAddMenuEntry("�ļ�����", 5);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);//���˵��������Ҽ�������
	glutReshapeFunc(reshapeFcn);
	glutMainLoop();
	return 0;
}
