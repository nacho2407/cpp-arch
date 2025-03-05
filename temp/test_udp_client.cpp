#include <boost/asio.hpp>

#include <array>
#include <iostream>
#include <thread>

using boost::asio::ip::udp;

class UdpClient
{
public:
        UdpClient(boost::asio::io_context &io_context, const std::string &host, short port)
            : socket_(io_context, udp::endpoint(udp::v4(), 0)), resolver_(io_context), sender_endpoint_(*resolver_.resolve(host, std::to_string(port)).begin())
        {
                start();
        }

        void start()
        {
                std::thread read_thread([this](void) {
                        std::array<char, 1024> recv_buffer_;
                        socket_.async_receive_from(boost::asio::buffer(recv_buffer_), receiver_endpoint_,
                        [this, &recv_buffer_](boost::system::error_code ec, std::size_t bytes_received) {
                                if (!ec && bytes_received > 0)
                                {
                                        std::cout << "Received: " << std::string(recv_buffer_.data(), bytes_received) << std::endl;
                                        start(); // 계속해서 메시지를 받기 위한 호출
                                }
                        });
                });

                std::string message;
                while(std::getline(std::cin, message))
                { // 표준 입력에서 메시지 읽기
                        socket_.async_send_to(boost::asio::buffer(message), sender_endpoint_);
                }

                socket_.shutdown(udp::socket::shutdown_both);
                socket_.close();

                read_thread.join();
        }

private:
        udp::socket socket_;
        udp::resolver resolver_;
        udp::endpoint sender_endpoint_;
        udp::endpoint receiver_endpoint_;
};

int main()
{
        try
        {
                boost::asio::io_context io_context;
                UdpClient client(io_context, "127.0.0.1", 12345); // 127.0.0.1:12345로 메시지 전송

                std::cout << "UDP client sending message..." << std::endl;

                io_context.run();
        }
        catch (std::exception &e)
        {
                std::cerr << "Exception: " << e.what() << std::endl;
        }

        return 0;
}
