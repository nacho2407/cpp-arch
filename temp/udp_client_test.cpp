#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::udp;

class ChatClient
{
public:
    ChatClient(boost::asio::io_context &io_context, const std::string &host, short port)
        : socket_(io_context, udp::endpoint(udp::v4(), 0)), server_endpoint_(*boost::asio::ip::udp::resolver(io_context).resolve(host, std::to_string(port)).begin())
    {
        receive();
    }

    void send(const std::string &message)
    {
        socket_.async_send_to(
            boost::asio::buffer(message), server_endpoint_,
            [](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {});
    }

private:
    void receive()
    {
        socket_.async_receive_from(
            boost::asio::buffer(data_, max_length), sender_endpoint_,
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::cout << "Received: " << std::string(data_, length) << "\n";
                }
                receive();
            });
    }

    udp::socket socket_;
    udp::endpoint server_endpoint_;
    udp::endpoint sender_endpoint_;
    enum
    {
        max_length = 1024
    };
    char data_[max_length];
};

int main()
{
    try
    {
        boost::asio::io_context io_context;

        ChatClient client(io_context, "127.0.0.1", 12345);

        std::thread io_thread([&io_context]() { io_context.run(); });

        std::string message;
        while (std::getline(std::cin, message))
        {
            client.send(message);
        }

        io_thread.join();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
