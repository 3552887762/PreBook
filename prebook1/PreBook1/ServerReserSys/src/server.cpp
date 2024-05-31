
#include<iostream>
using namespace std;


int main()
{
    MyLibevent *plib = new MyLibevent();
    if (plib == NULL)
    {
        cout << "create libevent err" << endl;
        exit(1);
    }

    if (!plib->MyLibevent_Init())
    {
        exit(1);
    }

    if (!plib->MYLibevent_Dispatch())
    {
        exit(1);
    }

    delete plib;

    exit(0);
}