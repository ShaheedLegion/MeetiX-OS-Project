#include <iostream>
using namespace std;

class Base
{
protected:
	void deallocate() { delete this; }
public:
	Base() {}
};

class Resultant : public Base
{
public:
	Resultant() {}
	
	void ciao() 
	{ 
		delete this;
		cout << "ciao" << endl; 
	}
};

int main()
{
	Resultant *s = new Resultant();
	s->ciao();
	s->ciao();
}
