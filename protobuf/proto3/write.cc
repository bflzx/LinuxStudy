#include <iostream>
#include <fstream>
#include "contacts.pb.h"

using namespace std;

void AddPeopleInfo(contacts2::PeopleInfo* people)
{
    cout << "----------------新增联系人--------------" << endl;
    cout << "请输入联系人姓名:";
    string name;
    getline(cin, name);
    people->set_name(name);

    cout << "请输入联系人年龄:";
    int age;
    cin >> age;
    people->set_age(age);
    cin.ignore(256,'\n');
    for (int i = 0;;i++)
    {
        cout << "请输入联系人电话" << i + 1 << "(只输入回车完成输入):";
        string number;
        getline(cin, number);
        if(number.empty())
            break;
        contacts2::Phone *phone = people->add_phone();
        phone->set_number(number);

        cout << "请输入该电话类型(1.移动电话 2.固定电话):";
        int type;
        cin >> type;
        cin.ignore('\n');
        switch(type)
        {
            case 1:
                phone->set_type(contacts2::Phone_PhoneType::Phone_PhoneType_MP);
                break;
            case 2:
                phone->set_type(contacts2::Phone_PhoneType::Phone_PhoneType_TEL);
            default:
                cout << "选择有误!" << endl;
        }
    }
    contacts2::Address address;
    cout << "请输入联系人家庭地址:";
    string home_address;
    getline(cin, home_address);
    address.set_home_address(home_address);
    cout << "请输入联系人单位地址:";
    string unit_address;
    getline(cin, unit_address);
    address.set_unit_address(unit_address);
    people->mutable_data()->PackFrom(address);

    cout << "---------------新增联系人成功------------" << endl;
}

int main()
{
    contacts2::Contacts contacts;
    // 读取本地已存在的联系人文件
    fstream input("contacts.bin", ios::in | ios::binary);
    if(!input)
    {
        cout << "contacts.bin not find,create new file" << endl;
    }
    else if(!contacts.ParseFromIstream(&input))
    {
        cout << "Parse error" << endl;
        input.close();
        return -1;
    }

    //向通讯录中添加一个联系人
    AddPeopleInfo(contacts.add_contacts());

    //将通讯录写入本地文件中
    fstream output("contacts.bin", ios::out | ios::trunc | ios::binary);
    if(!contacts.SerializeToOstream(&output))
    {
        cout << "write error" << endl;
        input.close();
        output.close();
        return -1;
    }
    cout << "write sucess!" << endl;
    return 0;
}