#include "pch.h"
#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <cmath>
#include <mmsystem.h>
#define PI 3.141592653
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "Msimg32.lib")
using namespace std;
IMAGE player1,player2;
IMAGE title, title1;
void mul(double mat[], double vec[])
{
	double t = vec[0];
	double t1 = vec[1];
	vec[0] = mat[0] * t + mat[2] * t1;
	vec[1] = mat[1] * t + mat[3] * t1;
}
bool detect(double x, double y)
{
	if (x < 0 || x>640 || y < 0 || y>720)
		return true;
	return false;
}

class Player
{
public:
	Player(int x1, int y1)
	{
		x = x1;
		y = y1;
	}
	void draw() 
	{
		loadimage(&player1, _T("player/player01.png"));
		loadimage(&player2, _T("player/player01_a.png"));
		putimage(x, y, &player2, NOTSRCERASE);
		putimage(x, y, &player1, SRCINVERT);
	}
	void move(char key)
	{
			switch (key)//获取按键
			{
				//向上
			case 72:
			case 'w':
			case 'W':
				if (y >= 0)
				{
					y -= 3;
				}
				break;
				//向下
			case 80:
			case 's':
			case 'S':
				if (y <= 650)
				{
					y += 3;
				}
				break;
				//向右
			case 77:
			case 'd':
			case 'D':
				if (x <= 610)
				{
					x += 3;
				}
				break;
				//向左
			case 75:
			case 'a':
			case 'A':
				if (x >= 0)
				{
					x -= 3;
				}
				break;
			}
	}
	int getcx()
	{
		return x + 15;
	}
	int getcy()
	{
		return y + 15;
	}
	int getx() { return x; }
	int gety() { return y; }
	int hp = 3;
	int heart()
	{
		return hp--;
	}
private:
	int x;
	int y;
	int cx;
	int cy;
	IMAGE player1, player2;

};
class Danmu 
{ 
public: virtual void output() = 0;
		virtual void action() = 0;
		virtual bool exitCondition() 
		{
			return false;
		}
		virtual bool crash(Player p) { return false; };
		//virtual double get_cx() {};
		//virtual double get_cy() {};
};
Danmu ** danmuku;
int danmuLen = 0;

void pushDanmu(Danmu ** dan, int len)
{
	int l = danmuLen + len;
	Danmu ** t = new Danmu*[l];
	for (int i = 0; i < danmuLen; ++i)
	{
		t[i] = danmuku[i];
	}
	for (int i = danmuLen; i < l; ++i)
	{
		t[i] = dan[i - danmuLen];
	}
	danmuLen += len;
	delete[] danmuku;
	danmuku = t;
}


void popDanmu(int len)
{
	danmuLen -= len;
	Danmu ** t = new Danmu*[danmuLen];
	for (int i = danmuLen - 1; i >= 0; --i)
	{
		t[i] = danmuku[len + i];
	}
	delete[] danmuku;
	danmuku = t;
}

class DanmuGroup : public Danmu
{
	Danmu ** danmus;
	int num;
public:
	DanmuGroup(Danmu ** d, int n)
	{
		danmus = d;
		num = n;
	}
	void output()
	{
		for (int i = 0; i < num; ++i)
		{
			danmus[i]->output();
		}
	}
	void action()
	{
		for (int i = 0; i < num; ++i)
		{
			danmus[i]->action();
		}
		if (exitCondition())
			exit();
	}
	bool exitCondition()
	{
		int flag = 0;
		for (int i = 0; i < num; ++i)
		{
			if (danmus[i]->exitCondition())
			{
				++flag;
			}
		}
		return flag == num;
	}
	void exit()
	{
		popDanmu(1);
	}
	bool crash(Player p)
	{
		for(int i = 0; i < num; ++i)
		{
			if (danmus[i]->crash(p))
			{
				return true;
			}
		}
		return false;
	}
};
class Bullet : public Danmu 
{
public:
	Bullet(){};
	void setBullet(double x0, double y0, double v[],int t)
	{
		type = t;
		x = x0;
		y = y0;
		vec[0] = v[0];
		vec[1] = v[1];
		mat[0] = cos(0.005);
		mat[1] = sin(0.005);
		mat[2] = -sin(0.005);
		mat[3] = cos(0.005);
	}
	void output()
	{
		if (type == 1)
		{
			loadimage(&bullet1, _T("bullet/3.png"));
			loadimage(&bullet2, _T("bullet/4.png"));
			putimage((int)x, (int)y, &bullet2, NOTSRCERASE);
			putimage((int)x, (int)y, &bullet1, SRCINVERT);
		}
		else if (type == 2)
		{
			loadimage(&sm1, _T("bullet/sm.png"));
			loadimage(&sm2, _T("bullet/2.png"));
			putimage((int)x, (int)y, &sm2, NOTSRCERASE);
			putimage((int)x, (int)y, &sm1, SRCINVERT);
		}
	}
	void action()
	{
		mul(mat, vec);
		//vec[0] += 1 / (200 * sqrt(vec[0]));
		x -= vec[0];
		y -= vec[1];
	}
	double get_cx()
	{
		return cx + 16;
	}
	double get_cy()
	{
		return cy + 15;
	}
	bool exitCondition()
	{
		return detect(x, y);
	}
	bool crash(Player p)
	{
		if (abs(get_cx() - p.getcx()) < 5 && abs(get_cy() - p.getcy()) < 5)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	double x;
	double y;
	double cx;
	double cy;
	double vec[2];
	double mat[4];
private:
	IMAGE bullet1;
	IMAGE bullet2;
	IMAGE sm1;
	IMAGE sm2;
	int type;
};
class Snow :public Danmu
{
public:
	Snow() {};
	void setSnow(double x0, double y0,double v[])
	{
		x = x0;
		y = y0;
		vec[0] = v[0];
		vec[1] = v[1];
	}
	void output()
	{
		loadimage(&snow1, _T("bullet/snow1.png"));
		loadimage(&snow2, _T("bullet/snow2.png"));
		putimage((int)x, (int)y, &snow2, NOTSRCERASE);
		putimage((int)x, (int)y, &snow1, SRCINVERT);
	}
	int flag;
	void setFlag(int a) { flag = a; }
	void action()
	{
		x += vec[0];
		y += vec[1];
	}
	double get_cx()
	{
		return cx + 8;
	}
	double get_cy()
	{
		return cy + 8;
	}
	bool exitCondition()
	{
		return detect(x, y);
	}
	bool crash(Player p)
	{
		if (abs(get_cx()-p.getcx())<5&& abs(get_cy() - p.getcy())<5)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	double x;
	double y;
	double cx, cy;
	double vec[2];
	double mat[4];
private:
	IMAGE snow1;
	IMAGE snow2;

};
class Sword :public Danmu
{
public:
	Sword() {};
	void output()
	{
		if (type==1)
		{
			loadimage(&sw1, _T("bullet/sw1.png"));
			loadimage(&sw2, _T("bullet/sw2.png"));
			putimage((int)x, (int)y, &sw2, NOTSRCERASE);
			putimage((int)x, (int)y, &sw1, SRCINVERT);
		}
		else if (type == 2)
		{
			loadimage(&snow3, _T("bullet/snow3.png"));
			loadimage(&snow4, _T("bullet/2.png"));
			putimage((int)x, (int)y, &snow3, NOTSRCERASE);
			putimage((int)x, (int)y, &snow4, SRCINVERT);
		}
	}
	void setSword(double x0, double y0, double v[],int t)
	{
		type = t;
		x = x0;
		y = y0;
		vec[0] = v[0];
		vec[1] = v[1];
	}
	void action()
	{
		x += vec[0];
		y += vec[1];
	}
	double get_cx()
	{
		return cx + 16;
	}
	double get_cy()
	{
		return cy + 16;
	}
	bool exitCondition()
	{
		return detect(x, y);
	}
	bool crash(Player p)
	{
		if (abs(get_cx() - p.getcx()) < 5 && abs(get_cy() - p.getcy()) < 5)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	double x;
	double y;
	double cx, cy;
	double vec[2];
	double mat[4];
private:
	int type;
	IMAGE sw1;
	IMAGE sw2;
	IMAGE snow3;
	IMAGE snow4;
};
class Fire : public Danmu
{
public:
	Fire() {};
	void setFire(double x0,double y0)
	{
		x = x0;
		y = y0;
		vec[0] = 2;
		vec[1] = 2;
		mat[0] = cos(0.01);
		mat[1] = sin(0.01);
		mat[2] = -sin(0.01);
		mat[3] = cos(0.01);
	}
	void output()
	{
		loadimage(&fire1, _T("bullet/fire1.png"));
		loadimage(&fire2, _T("bullet/fire2.png"));
		putimage((int)x, (int)y, &fire2, NOTSRCERASE);
		putimage((int)x, (int)y, &fire1, SRCINVERT);
	}
	int flag;
	void setFlag(int a) { flag = a; }
	void action() 
	{ 
		if (flag == 1)
		{
			action1();
		}
		else if (flag == 2)
		{ 
			action2();
		}
	}
	void action1()
	{
		mul(mat, vec);
		double v[] = { x, y };
		if (y <= 360)
		{
			x += vec[0];
			y += vec[1];
		}
		else
		{
			x -= vec[0];
			y += vec[1];
		}
	}
	void action2()
	{
		mul(mat, vec);
		output();
		double v[] = { x, y };
		if (y <= 360)
		{
			x -= vec[0];
			y += vec[1];
		}
		else
		{
			x += vec[0];
			y += vec[1];
		}

	}
	bool exitCondition()
	{
		return detect(x, y);
	}
	double get_cx()
	{
		return cx + 16;
	}
	double get_cy()
	{
		return cy + 15;
	}
	bool crash(Player p)
	{
		if (abs(get_cx() - p.getcx()) < 5 && abs(get_cy() - p.getcy()) < 5)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	double x;
	double y;
	double cx;
	double cy;
	double vec[2];
	double mat[4];
private:
	IMAGE fire1;
	IMAGE fire2;
};
void setSnow(int num, Snow * s, double x,double y, double r)
{
	for (int i = 0; i < num; i++)
	{
		double v[2] = { cos(2 * PI / num * i),sin(2 * PI / num * i) };
		s[i].setSnow(x + r * v[0], y + r * v[1], v);
	}
}
void setSword(int num, Sword * s, double x, double y, double r,int type)
{
	for (int i = 0; i < num; i++)
	{
		double v[2] = { cos(2 * PI / num * i),sin(2 * PI / num * i) };
		s[i].setSword(x, y, v,type);
	}
}
void setFire(int num, Fire *f, double x, double y)
{
	for (int i = 0; i < num; i++)
	{
		f[i].setFire(x, y+i * 16);
	}
}
void setBullet(int num, Bullet*b, double x, double y,double r,int type)
{
	for (int i = 0; i < num; i++)
	{
		double v[2] = { cos(2 * PI / num * i),sin(2 * PI / num * i) };
		b[i].setBullet(x + r * v[0], y + r * v[1], v,type);
	}
}
Player no1(320, 650);
Fire f1[21], f2[21], f3[21], f4[21];
Snow s1[30], s2[30], s3[30];
Sword sw1[20], sw2[20], sw3[20], sw4[20], sw5[20], sw6[20];
Sword sw7[20], sw8[20],sw9[20],sw10[20];
Bullet b1[20], b2[20], b3[20], b4[20];
Danmu ** bb1 = new Danmu*[20];
Danmu ** bb2 = new Danmu*[20]; 
Danmu ** bb3 = new Danmu*[20];
Danmu ** bb4 = new Danmu*[20];
Danmu ** ff1 = new Danmu*[21];
Danmu ** ff2 = new Danmu*[21];
Danmu ** ff3 = new Danmu*[21];
Danmu ** ff4 = new Danmu*[21];
Danmu ** ss1 = new Danmu*[30];
Danmu ** ss2 = new Danmu*[30];
Danmu ** ss3 = new Danmu*[30];
Danmu ** ssw1 = new Danmu*[20];
Danmu ** ssw2 = new Danmu*[20];
Danmu ** ssw3 = new Danmu*[20];
Danmu ** ssw4 = new Danmu*[20];
Danmu ** ssw5 = new Danmu*[20];
Danmu ** ssw6 = new Danmu*[20];
Danmu ** ssw7 = new Danmu*[20];
Danmu ** ssw8 = new Danmu*[20];
Danmu ** ssw9 = new Danmu*[20];
Danmu ** ssw10 = new Danmu*[20];
Danmu ** bbb1 = new Danmu*[1];
Danmu ** bbb2 = new Danmu*[1];
Danmu ** bbb3 = new Danmu*[1];
Danmu ** bbb4 = new Danmu*[1];
Danmu ** fff1 = new Danmu*[1];
Danmu ** fff2 = new Danmu*[1];
Danmu ** fff3 = new Danmu*[1];
Danmu ** fff4 = new Danmu*[1];
Danmu ** sss1 = new Danmu*[1];
Danmu ** sss2 = new Danmu*[1];
Danmu ** sss3 = new Danmu*[1];
Danmu ** sssw1 = new Danmu*[1];
Danmu ** sssw2 = new Danmu*[1];
Danmu ** sssw3 = new Danmu*[1];
Danmu ** sssw4 = new Danmu*[1];
Danmu ** sssw5 = new Danmu*[1];
Danmu ** sssw6 = new Danmu*[1];
Danmu ** sssw7 = new Danmu*[1];
Danmu ** sssw8 = new Danmu*[1];
Danmu ** sssw9 = new Danmu*[1];
Danmu ** sssw10 = new Danmu*[1];



void illustrate()
{
	for (int  i = 0; i < 40;i++)
	{
		cout << "*";
	}
	cout << "\n";
	cout << "***************游戏介绍*****************\n";
	cout << "**          仿stg类弹幕游戏           **\n"
		 << "**        游戏素材来自东方红乡        **\n"
		 << "**        进入游戏请按enter键         **\n"
		 << "****************************************\n";
}

void initial()
{
	IMAGE background1;
	IMAGE background2;
	IMAGE red1, red2;
	loadimage(&background1, _T("background/background1.jpg"));
	loadimage(&red1, _T("background/red1.png"));
	loadimage(&red1, _T("background/red2.png"));
	loadimage(&title, _T("background/title.png"));
	loadimage(&title1, _T("backgrouond/title_a.png"));
	initgraph(background1.getwidth(),background1.getheight());
	putimage(0, 0, &background1);
	putimage(220, 60, &title1, NOTSRCERASE);
	putimage(220, 60, &title, SRCINVERT);
	putimage(410, 60, &red2, NOTSRCERASE);
	putimage(410, 60, &red1, SRCINVERT);
}

void setDanmu()
{
	setSnow(30, s1, 320, 360, 1);
	setSnow(30, s2, 320, 360, 20);
	setSnow(30, s3, 320, 360, 50);
	setSword(20, sw1, 140, 240, 20,1); setSword(20, sw2, 120, 240, 20,1);
	setSword(20, sw3, 20, 480, 20,1);	setSword(20, sw4, 510, 240, 20,1);
	setSword(20, sw5, 480, 240, 20,1); setSword(20, sw6, 600, 480, 20,1);
	setSword(20, sw7, 120, 240, 20, 2); setSword(20, sw8, 540, 240, 20, 2);
	setSword(20, sw9, 120, 480, 20, 2); setSword(20, sw10, 540, 480, 20, 2);
	setFire(21, f1, 200, 0);setFire(21, f2, 400, 0);
	setFire(21, f3, 0, 0);setFire(21, f4, 600, 0);
	setBullet(20, b1, 320, 360, 5,1);setBullet(20, b2, 320, 360, 50,1);
	setBullet(20, b3, 320, 360, 5, 2); setBullet(20, b4, 320, 360, 50, 2);
	for (int i = 0; i < 21; ++i)
	{
		ff1[i] = &f1[i];
		ff2[i] = &f2[i];
		ff3[i] = &f3[i];
		ff4[i] = &f4[i];
		f1[i].setFlag(i % 2 == 1 ? 1 : 2);
		f2[i].setFlag(i % 2 == 1 ? 1 : 2);
		f3[i].setFlag(i % 2 == 1 ? 1 : 2);
		f4[i].setFlag(i % 2 == 1 ? 1 : 2);
	}
	for (int i = 0; i < 20; ++i)
	{
		ssw1[i] = &sw1[i];
		ssw2[i] = &sw2[i];
		ssw3[i] = &sw3[i];
		ssw4[i] = &sw4[i];
		ssw5[i] = &sw5[i];
		ssw6[i] = &sw6[i];
		ssw7[i] = &sw7[i];
		ssw8[i] = &sw8[i];
		ssw9[i] = &sw9[i];
		ssw10[i] = &sw10[i];
		bb1[i] = &b1[i];
		bb2[i] = &b2[i];
		bb3[i] = &b3[i];
		bb4[i] = &b4[i];
	}
	for (int i = 0; i < 30; i++)
	{
		ss1[i] = &s1[i];
		ss2[i] = &s2[i];
		ss3[i] = &s3[i];
	}
	sssw1[0] = new DanmuGroup(ssw1, 20);
	sssw2[0] = new DanmuGroup(ssw2, 20);
	sssw3[0] = new DanmuGroup(ssw3, 20);
	sssw4[0] = new DanmuGroup(ssw4, 20);
	sssw5[0] = new DanmuGroup(ssw5, 20);
	sssw6[0] = new DanmuGroup(ssw6, 20);
	sssw7[0] = new DanmuGroup(ssw7, 20);
	sssw8[0] = new DanmuGroup(ssw8, 20);
	sssw9[0] = new DanmuGroup(ssw9, 20);
	sssw10[0] = new DanmuGroup(ssw10, 20);
	sss1[0] = new DanmuGroup(ss1, 30);
	sss2[0] = new DanmuGroup(ss2, 30);
	sss3[0] = new DanmuGroup(ss3, 30);
	fff1[0] = new DanmuGroup(ff1, 21);
	fff2[0] = new DanmuGroup(ff2, 21);
	fff3[0] = new DanmuGroup(ff3, 21);
	fff4[0] = new DanmuGroup(ff4, 21);
	bbb1[0] = new DanmuGroup(bb1, 20);
	bbb2[0] = new DanmuGroup(bb2, 20);
	bbb3[0] = new DanmuGroup(bb3, 20);
	bbb4[0] = new DanmuGroup(bb4, 20);
}

void fresh()
{
	IMAGE background2;
	loadimage(&background2, _T("background/background2.jpg"));
	putimage(0, 0, &background2);
	no1.draw();
	for (int i = 0; i < danmuLen; ++i)
	{
		danmuku[i]->output();
	}
}

void start()
{
	int i = 0;
	for (;; i++)
	{
		BeginBatchDraw();
		if (no1.hp==0)
		{
			break;
		}
		if (_kbhit())
		{
			no1.move(_getch());
		}

		for (int i = 0; i < danmuLen; ++i)
		{
			danmuku[i]->action();
		}
		for (int i = 0; i < danmuLen; ++i)
		{
			if (danmuku[i]->crash(no1))
			{
				no1.heart();
				break;
			}
		}
		if (i == 200)
		{
			pushDanmu(fff1, 1);
			pushDanmu(fff2, 1);
			pushDanmu(fff3, 1);
			pushDanmu(fff4, 1);
			
		}
		else if (i == 300)
		{
			pushDanmu(sss1, 1);
		}
		else if (i == 500)
		{
			pushDanmu(sssw1, 1);
			pushDanmu(sssw4, 1);
		}
		else if (i == 700)
		{
			pushDanmu(sssw2, 1);
			pushDanmu(sssw3, 1);
			pushDanmu(sssw5, 1);
			pushDanmu(sssw6, 1);
		}
		else if (i == 1500)
		{
			pushDanmu(sss2, 1);
			pushDanmu(sss3, 1);
		}
		else if (i == 1700)
		{
			pushDanmu(bbb1, 1);
			pushDanmu(bbb2, 1);
		}
		else if (i == 2970)
		{
			popDanmu(2);
		}
		else if (i == 2000)
		{
			pushDanmu(bbb3, 1);
			pushDanmu(bbb4, 1);
		}
		else if (i == 2500)
		{
			pushDanmu(sssw7, 1);
			pushDanmu(sssw8, 1);
			pushDanmu(sssw9, 1);
			pushDanmu(sssw10, 1);
		}
		else if (i == 4000)
		{
			break;
		}

		fresh();
		FlushBatchDraw();
	}
}



void main()
{
	setDanmu();
	illustrate();
	_getch();
	initial();
	mciSendString(_T("open bgmusic.mid alias mymusic"), NULL, 0, NULL);
	mciSendString(_T("play mymusic"), NULL, 0, NULL);
	if (_getch() == 13)
	{
		start();
	}
	initial();
	_getch();
	closegraph();
}