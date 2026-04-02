#include <iostream>
#include <iostream>
#include "contacts.pb.h"

int main()
{
    contacts::PeopleInfo people;
    people.set_name("张三");
    people.set_age(20);
    //序列化
    std::string people_str;
    if(!people.SerializeToString(&people_str))
    {
        std::cout << "序列化失败" << std::endl;
        return -1;
    }
    std::cout << "序列化成功:" << people_str << std::endl;

    //反序列化
    contacts::PeopleInfo unpeople;
    if(!unpeople.ParseFromString(people_str))
    {
        std::cout << "反序列化失败!" << std::endl;
        return -1;
    }
    std::cout << "反序列化成功!" << std::endl;
    std::cout << "姓名:" << unpeople.name() << std::endl;
    std::cout << "年龄:" << unpeople.age() << std::endl;
    return 0;
}