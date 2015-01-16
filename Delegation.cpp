Delegation makes composition as powerful for reuse as inheritance.
With delegation, two objects are involved in handling a request.
A receiving object delegates operations to its delegate, which is 
similar to the case when subclass defers requests to parent class.

#include <iostream>
using namespace std; 

class Rectangle
{
private:
	double height, width;
public:
	Rectangle(double h, double w) {
		height = h;
		width = w;
	}
	double area() {
		cout << "Area of Rect. Window = ";
		return height*width;
	}
};

class Window 
{
public: 
	Window(Rectangle *r) : rectangle(r){}
	double area() {
		return rectangle->area();
	}
private:
	Rectangle *rectangle;
};


int main() 
{
	Window *wRect = new Window(new Rectangle(10,20));
	cout << wRect->area();

	return 0;
}

The main advantage of delegation is that it makes it easy to compose behaviors 
at run-time. For example, the Window can become circular at run-time as shown
in the following code.

#include <iostream>
using namespace std; 

class Shape
{
public:
	virtual double area() = 0;
};

class Rectangle : public Shape
{
private:
		double height, width;
public:
	Rectangle(double h, double w) {
		height = h;
		width = w;
	}
	double area() {
		return height*width;
	}
};

class Circle : public Shape
{
private:
		double radius;
public:
	Circle(double r) {
		radius = r;
	}
	double area() {
		return 3.14*radius*radius;
	}
};


class Window 
{
public: 
	Window (Shape *s):shape(s){}
	double area() {
		return shape->area();
	}
private:
	Shape *shape;
};


int main() 
{
	Window *wRect = new Window(new Rectangle(10,20));
	Window *wCirc = new Window(new Circle(20));
	cout << "rectangular Window:" << wRect->area() << endl;
	cout << "circular Window:" << wCirc->area() << endl;
	return 0;
}
