#ifndef DATA_H_
#define DATA_H_
#include<vector>
using namespace std;
class point //点类，存储了一个点的两坐标值
{
	public:
		int x;
		int y;
};

class polygon //多边形类，存了一个多边形
{
	public:
		vector<point> p; //多边形的顶点
		GLfloat red, green, blue;//用于保存每一个多边形所填充的颜色
};
class AETclass //活性边表类
{
public:
	double x;
	double dx;
	double ymax;
};

class SETclass   //生成边表类
{
public:
	double x;
	double dx;
	double ymax;
};
#endif
