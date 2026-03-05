// OCP Exercise - Fill in the blanks
// Open/Closed Principle

#include <iostream>
using namespace std;

//ans: 1 : Shape, 4 : public, 2: virtual, 3: draw, 5: 

class ___1___ 
{
public:
    ___2___ void ___3___() = 0;
};

class Circle : ___4___ ___1___
{
public:
    ___2___ void ___3___()
    {
        cout << "Draw Circle" << endl;
    }
};

class Square : ___4___ ___1___
{
public:
    ___2___ void ___3___()
    {
        cout << "Draw Square" << endl;
    }
};

class Pentagon : ___4___ ___1___
{
public:
    ___2___ void ___3___()
    {
        cout << "Draw Pentagon" << endl;
    }
};


int main()
{
    ___1___* s = new Circle();
    s->___3___();
    s = new Square();
    s->___3___();
    s = new Pentagon();
    s->___3___();
}

