#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <set>
#include <mutex>
#include <thread>

using boost::asio::ip::tcp;

class ChatSession : public std::enable_shared_from_this<ChatSession>
{
public:
	ChatSession(tcp::socket socket, boost::asio::thread_pool &pool, std::set<std::shared_ptr<ChatSession>> &clients, std::mutex &clients_mutex)
	    : socket_(std::move(socket)), pool_(pool), clients_(clients), clients_mutex_(clients_mutex) {}

	void start()
	{
		std::lock_guard<std::mutex> lock(clients_mutex_);
		clients_.insert(shared_from_this()); // 클라이언트 목록에 추가
		read();
	}

	void deliver(const std::string &message)
	{
		auto self(shared_from_this());
		boost::asio::post(pool_, [this, self, message]()
				  { boost::asio::async_write(socket_, boost::asio::buffer(message), [this, self](boost::system::error_code ec, std::size_t)
							     {
                        if(ec) {
                                stop();
                        } }); });
	}

private:
	void read()
	{
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
					[this, self](boost::system::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							std::string message(data_, length);
							broadcast(message);
							read(); // 다음 메시지 읽기
						}
						else
						{
							stop();
						}
					});
	}

	void broadcast(const std::string &message)
	{
		std::lock_guard<std::mutex> lock(clients_mutex_);
		for (auto &client : clients_)
		{
			client->deliver(message); // 모든 클라이언트에게 메시지 전송
		}
	}

	void stop()
	{
		std::lock_guard<std::mutex> lock(clients_mutex_);
		clients_.erase(shared_from_this()); // 클라이언트 목록에서 제거
		socket_.close();
	}

	tcp::socket socket_;
	boost::asio::thread_pool &pool_;
	std::set<std::shared_ptr<ChatSession>> &clients_;
	std::mutex &clients_mutex_;
	enum
	{
		max_length = 1024
	};
	char data_[max_length];
};

class ChatServer
{
public:
	ChatServer(boost::asio::io_context &io_context, short port, boost::asio::thread_pool &pool)
	    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), pool_(pool)
	{
		accept();
	}

private:
	void accept()
	{
		acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket)
				       {
            if (!ec) {
                auto session = std::make_shared<ChatSession>(std::move(socket), pool_, clients_, clients_mutex_);
                session->start();
            }
            accept(); });
	}

	tcp::acceptor acceptor_;
	boost::asio::thread_pool &pool_;
	std::set<std::shared_ptr<ChatSession>> clients_;
	std::mutex clients_mutex_;
};

int main()
{
	try
	{
		boost::asio::io_context io_context;
		boost::asio::thread_pool pool(std::thread::hardware_concurrency());

		ChatServer server(io_context, 12345, pool);

		std::cout << "Chat server started on port 12345\n";
		io_context.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
