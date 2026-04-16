#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;
using namespace boost::placeholders;

io_service service;
ip::tcp::endpoint ep(ip::address::from_string("192.168.195.134"),8080);


class talk_to_svr
{
public:
    talk_to_svr(const std::string& username):username_(username),sock_(service),started_(true)
    {}
    void connect(ip::tcp::endpoint ep)
    {
        sock_.connect(ep);
    }
    void loop()
    {
        write("login " + username_ + "\n");
        read_answer();
        while(started_)
        {
            write_request();
            read_answer();
            boost::this_thread::sleep(boost::posix_time::millisec(rand() % 7000));
        }
    }
    std::string username() const { return username_; }
    size_t read_complete(char* buf,boost::system::error_code& err,size_t bytes)
    {
        if(err) return 0;
        bool found = std::find(buf,buf + bytes,'\n') < buf + bytes;

        return found ? 0 : 1;
    }
    void write_request()
    {
        write("ping\n");
    }

    void read_answer()
    {
        already_read_ = 0;
        read(sock_,buffer(buff_),boost::bind(&talk_to_svr::read_complete,this,_1,_2));
        process_msg();
    }

    void process_msg()
    {
        std::string msg(buff_,already_read_);
        if(msg.find("login") == 0) on_login();
        else if(msg.find("ping") == 0) on_ping(msg);
        else if(msg.find("clients " == 0)) on_clients(msg);
        else std::cerr << "invalid msg" << msg << std::endl;
    }
    void on_login() { do_ask_clients(); }
    void on_ping(const std::string& msg)
    {
        std::istringstream in(msg);
        std::string answer;
        in >> answer >> answer;
        if(answer == "client_list_changed") do_ask_clients();
    }

    void on_clients(const std::string& msg)
    {
        std::string clients = msg.substr(8);
        std::cout << username_ << ", new client list: " << clients;
    }

    void do_ask_clients()
    {
        write("ask_clients\n");
        read_answer();
    }
    void write(const std::string& msg) { sock_.write_some(buffer(msg)); }
private:
    ip::tcp::socket sock_;
    static constexpr int max_msg = 1024;
    int already_read_;
    char buff_[max_msg];
    bool started_;
    std::string username_;
};

void run_client(const std::string& client_name)
{
    talk_to_svr client(client_name);
    try
    {
        client.connect(ep);
        client.loop();
    }
    catch(boost::system::system_error& err)
    {
        std::cout << "client terminated" << std::endl;
    }
}

int main()
{

    return 0;
}