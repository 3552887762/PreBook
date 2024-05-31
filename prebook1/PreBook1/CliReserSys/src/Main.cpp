
#include"../include/Client.hpp"
int main()
{
    Client cli;
    clog << "client main" << endl;
    if (!cli.ConnectToServer())
    {
        exit(1);
    }

    cli.Run();

    exit(0);
}