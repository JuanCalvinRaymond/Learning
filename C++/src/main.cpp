// #include <iostream>
// // #include "Print.H"
// #include <string>

// struct Entity
// {
//     // public
//     static int a;
//     int b = 5;

//     void Print()
//     {
//         static int c = 2;
//         int d = 3;
//         c++;
//         d++;
//         std::cout << "c: " << c << ", d: " << d << std::endl;
//     }
// };

// int Entity::a = 0;
// int main()
// {
//     Entity entity = Entity();

//     entity.Print();
//     entity.Print();
//     entity.Print();
//     entity.Print();
//     entity.Print();
//     return 0;
// }

#include <iostream>
#include <mutex>
#include <thread>

// std::mutex mtx;

// void print_block(int n, char c)
// {
//     {
//         std::unique_lock<std::mutex> locker(mtx);
//         for (int i = 0; i < n; ++i)
//         {
//             std::cout << c;
//         }
//         std::cout << '\n';
//     }
// }

int main()
{
    // std::thread t1(print_block, 50, '*');
    // std::thread t2(print_block, 50, '$');

    // t1.join();
    // t2.join();
    int a = 0;
    if (a == 0)
    {
        a = 1;
    }

    std::cout << a << std::endl;
    return 0;
}