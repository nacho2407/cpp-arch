#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <mutex>
#include <thread>

using boost::asio::ip::udp;

class ChatServer
{
public:
    ChatServer(boost::asio::io_context &io_context, short port, boost::asio::thread_pool &pool)
        : socket_(io_context, udp::endpoint(udp::v4(), port)), pool_(pool)
    {
        receive();
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
                    std::string message(data_, length);
                    {
                        std::lock_guard<std::mutex> lock(clients_mutex_);
                        clients_.insert(sender_endpoint_); // 클라이언트 주소 저장
                    }
                    broadcast(message); // 모든 클라이언트에게 전송
                }
                receive(); // 다음 메시지 수신 대기
            });
    }

    void broadcast(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (const auto &client : clients_)
        {
            socket_.async_send_to(
                boost::asio::buffer(message), client,
                [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
                {
                });
        }
    }

    udp::socket socket_;
    boost::asio::thread_pool &pool_;
    std::set<udp::endpoint> clients_;
    std::mutex clients_mutex_;
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
        boost::asio::thread_pool pool(std::thread::hardware_concurrency());

        ChatServer server(io_context, 12345, pool);

        std::cout << "UDP Chat server started on port 12345\n";
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
