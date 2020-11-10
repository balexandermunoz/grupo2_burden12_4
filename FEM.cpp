#include "include/Triangulation.h"
#include <algorithm>
#include <iostream> 

//prototipos

using namespace std;

int findNodeIndex(vector<Point> & nodes, Point & p);
//step 4-Nicole
void doubleIntegrals(Triangle &triangles, vector<vector<vector<double>>> &Z, vector<vector<double>> &H);

//step 5-Nicole
void lineIntegrals(Triangle & trianglesS2, vector<vector<vector<double>>> &J, vector<vector<double>> &I);

//step 6-12-Brayan
void assembleDoubleIntegrals(Triangle &t, vector<vector<double>> & alpha, vector<double> & beta, vector<double> &gamma, vector<vector<vector<double>>> &Z, vector<vector<double>> &H);

//funcion que calcula el delta (determinante) para el triángulo i:
double DeltaT( Triangle T )
{	
	double x1,x2,x3,y1,y2,y3;
	x1 = T.getE1().getX(); y1 = T.getE1().getY();
	x2 = T.getE2().getX(); y2 = T.getE2().getY();
	x3 = T.getE3().getX(); y3 = T.getE3().getY();
	
    double matrix[10][10] = {{1.0, x1 , y1},
			    { 1.0, x2 , y2},
			    { 1.0, x3 , y3}};

	double delta;
	delta = determinante(matrix, 3);
	cout<<"El determinante es: "<< delta<<endl;
	return delta;
}

//Coeficientes A del triángulo i
vector <double> coef_a( Triangle T, double delta )
{	
	double x1,x2,x3,y1,y2,y3;
	x1 = T.getE1().getX(); y1 = T.getE1().getY();
	x2 = T.getE2().getX(); y2 = T.getE2().getY();
	x3 = T.getE3().getX(); y3 = T.getE3().getY();

	double a1, a2, a3;
	a1 = (x2*y3 - y2*x3)/delta;
	a2 = (x3*y1 - y3*x1)/delta;
	a3 = (x1*y2 - y1*x2)/delta;
	vector <double> a = {a1,a2,a3};

	return a;
}

//Coeficientes B del triángulo i
vector <double> coef_b( Triangle T, double delta )
{	
	double x1,x2,x3,y1,y2,y3;
	x1 = T.getE1().getX(); y1 = T.getE1().getY();
	x2 = T.getE2().getX(); y2 = T.getE2().getY();
	x3 = T.getE3().getX(); y3 = T.getE3().getY();
	
	double b1, b2, b3;
	b1 = (y2 - y3)/delta;
	b2 = (y3 - y1)/delta;
	b3 = (y1 - y2)/delta;

	vector <double> b = {b1,b2,b3};

	return b;
}

//Coeficientes C del triángulo i
vector <double> coef_c( Triangle T, double delta )
{	
	double x1,x2,x3,y1,y2,y3;
	x1 = T.getE1().getX(); y1 = T.getE1().getY();
	x2 = T.getE2().getX(); y2 = T.getE2().getY();
	x3 = T.getE3().getX(); y3 = T.getE3().getY();

	double c1, c2, c3;
	c1 = (x3 - x2)/delta;
	c2 = (x1 - x3)/delta;
	c3 = (x2 - x1)/delta;

	vector <double> c = {c1,c2,c3};

	return c;
}

//N_i, recibe a, b, c y además x e y
vector <double> Ni( vector <double> &a, vector <double> &b, vector <double> &c, double x, double y)
{
	vector <double> N = {};
		for(int i = 0; i < 3; i++){
		N.push_back( a[i] + b[i]*x + c[i]*y );}
	return N;
}
	
//Todo en una sola (sobrecargo de la función Ni anterior)
vector <double> Ni( Triangle T, double x, double y)
{

	double delta = DeltaT( T );
	vector <double> a = coef_a( T, delta );
	vector <double> b = coef_b( T, delta );
	vector <double> c = coef_c( T, delta );
	vector <double> N = Ni( a, b, c, x, y);
	return N;
}

//step 13-19-Brayan
void assembleDoubleIntegrals(Triangle &trianglesS2, vector<vector<double>> & alpha, vector<double> & beta, vector<double> &gamma, vector<vector<vector<double>>> &J, vector<vector<double>> &I);

//step 8, 10, 15, 17
int findNodeIndex(vector<Point> & nodes, Point &p){
    int i = 0;
    while(i<nodes.size())
    {
        if(p.getX() == nodes.at(i).getX() && p.getY() == nodes.at(i).getY())
            break;
        i++;
    }
    if(i == nodes.size())
        i = -1;
    return i;
}

void solve(double* (*S2Fx)(const double &, double &), double* (*S2Fy)(double &, const double &), double (*g)(const double &, const double &), vector<double> & x_i, vector<double> &y_i, int &N, int &M)
{
    
    vector<Point> nodesS1;
    vector<Point> nodesS2;
    vector<Triangle> trianglesS1;
    vector<Triangle> trianglesS2;
    vector<Triangle> trianglesnotS1S2;
    
    vector<double> x;

    vector<double> y;

    Triangulation t;
    t.multi_linspace(x_i, N, x);
    t.multi_linspace(y_i, M, y);
    
    t.buildTrianglesAndNodes(trianglesS1, trianglesS2, trianglesnotS1S2, nodesS1, nodesS2, S2Fx, S2Fy, x, y);
    Point p(x[0], y[0]);
    
    string fname = "dat/S1Triangles.dat";
    t.saveTriangles(trianglesS1, fname);
    fname = "dat/S2Triangles.dat";
    t.saveTriangles(trianglesS2, fname);
    fname = "dat/NotS1S2Triangles.dat";
    t.saveTriangles(trianglesnotS1S2, fname);
    
    fname = "dat/nodesS1.dat";
    t.saveNodes(nodesS1, fname);
    fname = "dat/nodesS2.dat";
    t.saveNodes(nodesS2, fname);
    
    const int n = nodesS1.size();
    const int m = nodesS2.size();
    
    vector<double> gammaS1;
    
    for(int i = 0; i<n-1; i++)
    {
        gammaS1.push_back(g(nodesS1.at(i).getX(), nodesS1.at(i).getY()));
    }
    //step 2
    
    vector<vector<double>> alpha(n, vector<double> (n, 0));
    vector<double> beta(n, 0);
    
}
