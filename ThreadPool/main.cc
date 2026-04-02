#include "ThreadPool.hpp"
#include<memory>
#include<unistd.h>
#include"Task.hpp"

int main()
{
    // std::unique_ptr<ThreadPool<Task>> tp(new ThreadPool<Task>());
    // tp->run();
    ThreadPool<Task>::getInstance()->run();

    int x, y;
    char op;
    while (true)
    {
        std::cout << "请输入数据1# ";
        std::cin >> x;
        std::cout << "请输入数据2#";
        std::cin >> y;
        std::cout << "请输入你要进行的运算#";
        std::cin >> op;

        Task t(x, y, op,mymath);
        std::cout << "你刚刚录入了一个任务: " << t.toTaskString() <<",确认提交吗?[y/n]#";
        char confirm;
        std::cin >> confirm;
        if(confirm == 'y')
        {
            // tp->push(t);
            ThreadPool<Task>::getInstance()->push(t);
        }
        sleep(1);
    }
    return 0;
}