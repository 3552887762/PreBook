
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

int main() {  
    std::vector<UseRegNode> usevec;
    //while(1);
    usevec.reserve(UseNum + 2);
    for (int i = 104; i < UseNum+104;++i)
    {
        UseRegNode use;
        use.name = std::string("mzx") + std::to_string(i);
        use.tel = std::string("138888") + std::to_string(i);
        use.passwd = std::string("888") + std::to_string(i);
        usevec.push_back(use);
    }
    std::vector<Client> arr(10);  
    std::vector<std::thread> threads;  
  
    // 为每个Client对象启动一个线程来连接服务器  
    for(Client& x : arr) {  
        threads.emplace_back([&]() {  
        x.ConnectToServer();  
         for (auto &use : usevec)
    {
        
        clog << "tel: " << use.tel << " name: " << use.name << " passwd: " << use.passwd << endl;
        x.User_Register(use.tel, use.name, use.passwd);
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
        });  
    }  
  
    // 等待所有线程完成  
    for(auto& t : threads) {  
        t.join();  
    }  
  
    std::cout << "All clients have connected to the server" << std::endl;  
    return 0;  
}
#if 0
int main()
{
    std::vector<UseRegNode> usevec;
    //while(1);
    usevec.reserve(UseNum + 2);
    for (int i = 0; i < UseNum;++i)
    {
        UseRegNode use;
        use.name = std::string("mzx") + std::to_string(i);
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
        
        clog << "tel: " << x.tel << " name: " << x.name << " passwd: " << x.passwd << endl;
        cli.User_Register(x.tel, x.name, x.passwd);
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
#endif

#if 0
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