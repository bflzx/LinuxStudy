#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <memory>

using namespace std::placeholders;
using namespace boost::asio;

io_service service;

class connection : std::__enable_shared_from_this<connection>
{
public:
    typedef boost::system::error_code error_code;
    typedef std::shared_ptr<connection> ptr;

    connection():_sock(service),_started(true) {}
    void start(ip::tcp::endpoint ep)
    {
        _sock.async_connect(ep,std::bind(&connection::on_connect,shared_from_this(),_1));
    }
    void stop()
    {
        if(!_started) return;
        _started = false;
        _sock.close();
    }
    bool started() { return _started; };

private:
    void on_connect(const error_code& err)
    {
        // 这里决定用这个连接做什么:读取或写入
        if(!err) do_read();
        else stop();
    }

    void on_read(const error_code& err,size_t bytes)
    {
        if(!started()) return;
        std::string msg(_read_buffer,bytes);
        if(msg == "can_login") do_write("access_data");
        else if(msg.find("data ") == 0) process_data(msg);
        else if(msg == "login_fail") stop();
    }
    void on_write(const error_code& err,size_t bytes)
    {
        do_read();
    }
    void do_read()
    {
        _sock.async_read_some(buffer(_read_buffer),std::bind(&connection::on_read,shared_from_this(),_1,_2));
    }

    void do_write(const std::string& msg)
    {
        if(!started()) return;
        // 注意:因为在做另一个async_read操作之前你想要发送多个消息
        // 所以你需要多个写入buffer
        std::copy(msg.begin(),msg.end(),_write_buffer);
        _sock.async_write_some(buffer(_write_buffer,msg.size()),std::bind(&connection::on_write,shared_from_this(),_1,_2));
    }
    void process_data(const std::string& msg)
    {
        //处理服务端来的内容,然后启动另一个写入操作
    }
private:
    ip::tcp::socket _sock;
    enum {max_msg = 1024};
    char _read_buffer[max_msg];
    char _write_buffer[max_msg];
    bool _started;
};

int main(int argc,char* argv[])
{
    ip::tcp::endpoint ep(ip::address::from_string("192.168.195.134"),8080);
    connection::ptr(new connection)->start(ep);
    return 0;
}