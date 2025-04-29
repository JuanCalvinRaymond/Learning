#include <iostream>
#include "Print.H"
#include <string>

class Entity
{
    // public:
    int a = 5;

public:
    virtual int GetA()
    {
        return a;
    }
};

class Person : public Entity
{
    int a = 7;

    // public:
    //     virtual int GetA() override
    //     {
    //         return a;
    //     }
};

int main()
{
    Entity entity;
    Person person;

    std::cout << entity.GetA() << std::endl;
    std::cout << person.GetA() << std::endl;
    // std::string string = "Hello";
    // Log(string);

    return 0;
}