#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;

class ChatClient
{
public:
    ChatClient(boost::asio::io_context &io_context, const std::string &host, short port)
        : socket_(io_context)
    {
        // 서버에 연결
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        boost::asio::connect(socket_, endpoints);
    }

    void start()
    {
        std::thread read_thread(&ChatClient::read, this); // 수신 스레드 실행

        std::string message;
        while (std::getline(std::cin, message))
        { // 표준 입력에서 메시지 읽기
            send(message);
        }

        socket_.close();
        read_thread.join(); // 수신 스레드 종료 대기
    }

private:
    void send(const std::string &message)
    {
        boost::asio::write(socket_, boost::asio::buffer(message + "\n"));
    }

    void read()
    {
        try
        {
            char data[1024];
            while (true)
            {
                std::size_t length = socket_.read_some(boost::asio::buffer(data));
                std::cout << "Server: " << std::string(data, length) << std::endl;
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "Disconnected from server: " << e.what() << std::endl;
        }
    }

    tcp::socket socket_;
};

int main()
{
    try
    {
        boost::asio::io_context io_context;
        ChatClient client(io_context, "127.0.0.1", 12345);
        client.start();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
