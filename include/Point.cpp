#include <cmath>
#ifndef POINT_CPP
#define POINT_CPP
struct point{
    float x, y;
};
point operator+(point a,point b){
	point ans;
	ans.x = a.x + b.x;
	ans.y = a.y + b.y;
	return ans;
}
point operator-(point a,point b){
	point ans;
	ans.x = a.x - b.x;
	ans.y = a.y - b.y;
	return ans;
}
point operator/(point a,double b){
	point ans;
	ans.x = a.x/b;
	ans.y = a.y/b;
	return ans;
}
point operator*(point a,double b){
	point ans;
	ans.x = a.x*b;
	ans.y = a.y*b;
	return ans;
}
float _abs(float number){
	return number>=0?number:-number;
}
float distance(point A, point B) {
	return _abs(A.x-B.x) + _abs(A.y-B.y);
}
float mol(point A){
	return sqrt(A.x*A.x+A.y*A.y);
}
point towards(point from, point to, double speed) {
	point ans = to - from;
	return ans*speed / mol(ans);
}
point rotate(point A, double angle) {
	point ans;
	double rad = angle*3.14159/180;
	ans.x = A.x*cos(rad)-A.y*sin(rad);
	ans.y = A.y*cos(rad)+A.x*sin(rad);
	return ans;
}
#endif /* POINT */