
#include<vector>
#include<string>
#include<thread>
using namespace std;

#include"../include/Client.hpp"
const int UseNum = 10;
struct UseRegNode
{
    std::string tel;
    std::string name;
    std::string passwd;
};
#if 0
int main()
{
    std::vector<UseRegNode> usevec;
    while(1);
    usevec.reserve(UseNum + 2);
    for (int i = 0; i < UseNum;++i)
    {
        UseRegNode use;
        use.name = std::string("yhp") + std::to_string(i);
        use.tel = std::string("138888") + std::to_string(i);
        use.passwd = std::string("888") + std::to_string(i);
        usevec.push_back(use);
    }
    Client cli;
    clog << "client main" << endl;
    if (!cli.ConnectToServer())
    {
        exit(1);
    }
    for (auto &x : usevec)
    {
      //  clog << "tel: " << x.tel << " name: " << x.name << " passwd: " << x.passwd << endl;
        cli.User_Register(x.tel, x.name, x.passwd);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
#endif

#if 1 
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
#endif 