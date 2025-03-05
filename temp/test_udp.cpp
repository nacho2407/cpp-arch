#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

class UdpServer
{
public:
        UdpServer(boost::asio::io_context &io_context, short port)
            : socket_(io_context, udp::endpoint(udp::v4(), port))
        {
                start_receive();
        }

private:
        void start_receive()
        {
                socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                                           remote_endpoint_,
                                           [this](boost::system::error_code ec, std::size_t bytes_received)
                                           {
                                                   if (!ec && bytes_received > 0)
                                                   {
                                                           std::cout << "Received: " << std::string(recv_buffer_.data(), bytes_received) << std::endl;
                                                           start_receive(); // 계속해서 메시지를 받기 위한 호출
                                                   }
                                           });
        }

        udp::socket socket_;
        udp::endpoint remote_endpoint_;
        std::array<char, 1024> recv_buffer_;
};

int main()
{
        try
        {
                boost::asio::io_context io_context;
                UdpServer server(io_context, 12345); // 12345 포트에서 서버 시작

                std::cout << "UDP server is listening on port 12345..." << std::endl;
                io_context.run();
        }
        catch (std::exception &e)
        {
                std::cerr << "Exception: " << e.what() << std::endl;
        }

        return 0;
}
