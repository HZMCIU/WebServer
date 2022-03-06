#include "base/Logging.h"
#include "base/Thread.h"

#include <iostream>
#include <string>

using namespace std;
void aysnc_test()
{

}
void type_test()
{
    cout << "--------------------type test--------------------" << endl;
    LOG << 'a';
    LOG << (short)1;
    LOG << (unsigned short) 1;
    LOG << (int) -1234;
    LOG << (unsigned)1234;
    LOG << (long) -1234;
    LOG << (unsigned long) 1234;
    LOG << (long long) -1234;
    LOG << (unsigned long long)1234;
    LOG << 3.14f;
    LOG << 3.1415926;
    LOG << "abcedfgh";
}


int main(int argc, char *argv[])
{
    type_test();
    return 0;
}
