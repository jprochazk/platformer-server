#pragma once


#include "network/boost.h"
#include "network/socket_handler.h"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include <atomic>

namespace network {
namespace test {

	class client;

	class client_handler {
	public:
		virtual ~client_handler() = default;

		// Called when a socket is fully opened
		virtual void on_open() = 0;

		// Called when a socket is fully closed
		virtual void on_close() = 0;

		// Called when a message is received from a socket
		virtual void on_message(std::vector<uint8_t>&& data) = 0;

		// Called when the socket encounters an error
		// Sockets that got an error shouldn't be considered close until you receive an `on_close` event from it
		virtual void on_error(std::string_view what, beast::error_code error) = 0;
	};

	class client : public std::enable_shared_from_this<client>
	{
	public:
		client(
			net::io_context& ioc,
			std::string_view host,
			std::string_view port,
			std::shared_ptr<client_handler> chandler
			) : host_(host)
			, port_(port)
			, ws_(net::make_strand(ioc))
			, fbuffer_()
			, client_handler_(chandler)
			, writing_(false)
			, wbuffer_()
		{}

		~client()
		{
			client_handler_->on_close();
		}

		void open()
		{
			tcp::resolver resolver(ws_.get_executor());

			resolver.async_resolve(
				host_,
				port_,
				beast::bind_front_handler(
					&client::on_resolve,
					shared_from_this()));
		}

		void send(std::vector<uint8_t> data)
		{
			net::post(
				ws_.get_executor(),
				beast::bind_front_handler(
					&client::on_send,
					shared_from_this(),
					std::move(data)));
		}

		void close()
		{
			ws_.async_close(
				beast::websocket::close_code::normal,
				beast::bind_front_handler(
					&client::on_close,
					shared_from_this()));
		}

	private:
		void on_resolve(beast::error_code ec, tcp::resolver::results_type results)
		{
			if (ec) return client_handler_->on_error("on_resolve", ec);

			beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(10));

			beast::get_lowest_layer(ws_).async_connect(
				results,
				beast::bind_front_handler(
					&client::on_connect,
					shared_from_this()));
		}

		void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
		{
			if (ec) return client_handler_->on_error("on_connect", ec);

			beast::get_lowest_layer(ws_).expires_never();

			ws_.set_option(
				beast::websocket::stream_base::timeout::suggested(
					beast::role_type::client));

			ws_.set_option(beast::websocket::stream_base::decorator(
				[](beast::websocket::request_type& req)
			{
				req.set(http::field::user_agent,
					std::string("TEST_CLIENT"));
			}));

			ws_.binary(true);

			ws_.async_handshake(host_, "/",
				beast::bind_front_handler(
					&client::on_handshake,
					shared_from_this()));
		}

		void on_handshake(beast::error_code ec)
		{
			if (ec) return client_handler_->on_error("on_handshake", ec);

			client_handler_->on_open();

			ws_.async_read(
				fbuffer_,
				beast::bind_front_handler(
					&client::on_read,
					shared_from_this()));
		}

		void on_read(beast::error_code ec, std::size_t read_bytes)
		{
			if (ec) return client_handler_->on_error("on_read", ec);

			{
				std::vector<uint8_t> buf(read_bytes);
				boost::asio::buffer_copy(
					boost::asio::buffer(buf.data(), read_bytes),
					fbuffer_.data());
				client_handler_->on_message(std::move(buf));
			}

			fbuffer_.consume(read_bytes);

			ws_.async_read(
				fbuffer_,
				beast::bind_front_handler(
					&client::on_read,
					shared_from_this()));
		}

		void on_send(std::vector<uint8_t>&& data)
		{
			wbuffer_.push_back(std::move(data));

			if (writing_)
				return;

			writing_ = true;

			ws_.async_write(
				net::buffer(wbuffer_.front()),
				beast::bind_front_handler(
					&client::on_write,
					shared_from_this()));
		}

		void on_write(beast::error_code ec, std::size_t written_bytes)
		{
			if (ec) return client_handler_->on_error("on_write", ec);

			wbuffer_.erase(wbuffer_.begin());

			if (wbuffer_.empty()) {
				writing_ = false;
				return;
			}

			ws_.async_write(
				net::buffer(wbuffer_.front()),
				beast::bind_front_handler(
					&client::on_write,
					shared_from_this()));
		}

		void on_close(beast::error_code ec)
		{
			if (ec) return client_handler_->on_error("on_write", ec);
		}

		std::string host_;
		std::string port_;
		beast::websocket::stream<beast::tcp_stream> ws_;
		beast::flat_buffer fbuffer_;
		std::shared_ptr<client_handler> client_handler_;
		bool writing_;
		std::vector<std::vector<uint8_t>> wbuffer_;
	};

} // namespace test
} // namespace network

#include <gtest/gtest.h>
#include <map>
#include "network/socket_listener.h"
#include "network/socket.h"
#include <spdlog/spdlog.h>

class test_server_handler : public network::socket_handler {
public:
	std::function<void(uint32_t, std::weak_ptr<network::socket_base>)> on_open_fn;
	void set_on_open(std::function<void(uint32_t, std::weak_ptr<network::socket_base>)> fn) {
		on_open_fn = fn;
	}
	void on_open(uint32_t id, std::weak_ptr<network::socket_base> socket) override {
		if (on_open_fn) on_open_fn(id, socket);
	}

	std::function<void(uint32_t)> on_close_fn;
	void set_on_close(std::function<void(uint32_t)> fn) {
		on_close_fn = fn;
	}
	void on_close(uint32_t id) override {
		if (on_close_fn) on_close_fn(id);
	}

	std::function<void(uint32_t, std::vector<uint8_t>&&)> on_message_fn;
	void set_on_message(std::function<void(uint32_t,std::vector<uint8_t>&&)> fn) {
		on_message_fn = fn;
	}
	void on_message(uint32_t id, std::vector<uint8_t>&& data) override {
		if (on_message_fn) on_message_fn(id, std::move(data));
	}

	std::function<void(uint32_t, std::string_view, beast::error_code)> on_error_fn;
	void set_on_error(std::function<void(uint32_t,std::string_view,beast::error_code)> fn) {
		on_error_fn = fn;
	}
	void on_error(uint32_t id, std::string_view what, beast::error_code ec) override {
		if (on_error_fn) on_error_fn(id, what, ec);
	}
};
class test_client_handler : public network::test::client_handler {
public:
	std::function<void()> on_open_fn;
	void set_on_open(std::function<void()> fn) {
		on_open_fn = fn;
	}
	void on_open() override {
		if (on_open_fn) on_open_fn();
	}

	std::function<void()> on_close_fn;
	void set_on_close(std::function<void()> fn) {
		on_close_fn = fn;
	}
	void on_close() override {
		if (on_close_fn) on_close_fn();
	}

	std::function<void(std::vector<uint8_t>&&)> on_message_fn;
	void set_on_message(std::function<void(std::vector<uint8_t>&&)> fn) {
		on_message_fn = fn;
	}
	void on_message(std::vector<uint8_t>&& data) override {
		if (on_message_fn) on_message_fn(std::move(data));
	}

	std::function<void(std::string_view, beast::error_code)> on_error_fn;
	void set_on_error(std::function<void(std::string_view, beast::error_code)> fn) {
		on_error_fn = fn;
	}
	void on_error(std::string_view what, beast::error_code ec) override {
		if (on_error_fn) on_error_fn(what, ec);
	}
};

class network_test : public ::testing::Test {
public:
	std::atomic<bool> done;
	std::atomic<bool> success;

	net::io_context ioc;

	std::shared_ptr<test_server_handler> server_handler;
	std::shared_ptr<network::socket_listener> server;
	std::shared_ptr<test_client_handler> client_handler;
	std::shared_ptr<network::test::client> client;

	inline void finish(bool status) {
		success.store(status, std::memory_order_release);
		done.store(true, std::memory_order_release);
	}

	bool get_status() {
		return success.load(std::memory_order_acquire);
	}

	network_test()
		: done(false)
		, success(false)
		, ioc()
	{
		server_handler = std::make_shared<test_server_handler>();
		server = std::make_shared<network::socket_listener>(
			ioc, tcp::endpoint{ net::ip::make_address("127.0.0.1"), 8001 },
			server_handler);
		client_handler = std::make_shared<test_client_handler>();
		client = std::make_shared<network::test::client>(ioc, "127.0.0.1", "8001", client_handler);
	}
};

std::shared_ptr<std::thread> dispatch_test_task(net::io_context& ioc, std::function<void()> fn) {
	return std::shared_ptr<std::thread>(new std::thread([&ioc, fn] {
		fn();
		ioc.run();
	}), [&ioc](std::thread* thread) {
		ioc.stop();
		thread->join();
		delete thread;
	});
}

bool wait_for(
	network_test* context, 
	std::chrono::duration<uint64_t> timeout = std::chrono::seconds(5)) 
{
	auto start = std::chrono::steady_clock::now();
	while (!context->done.load(std::memory_order_acquire)) {
		if (std::chrono::steady_clock::now() >= start + timeout) {
			spdlog::info("timed out");
			return false;
		}
	}

	return context->get_status();
}

TEST_F(network_test, network_connection) {
	auto handle = dispatch_test_task(ioc, [this]() {
		server->open();
		client->open();

		client_handler->set_on_error([this](std::string_view what, beast::error_code ec) {
			static const auto start = std::chrono::steady_clock::now();
			static const auto timeout = std::chrono::seconds(2);
			if (std::chrono::steady_clock::now() >= start + timeout) {
				spdlog::error("{}: {}", what, ec.message());
			}
			else {
				client->open();
			}
		});
		client_handler->set_on_open([this]() {
			finish(true);
		});
	});

	EXPECT_TRUE(wait_for(this));
}

TEST_F(network_test, network_client_receive) {
	auto handle = dispatch_test_task(ioc, [this] {
		server->open();
		client->open();

		client_handler->set_on_error([this](std::string_view what, beast::error_code ec) {
			static const auto start = std::chrono::steady_clock::now();
			static const auto timeout = std::chrono::seconds(2);
			if (std::chrono::steady_clock::now() >= start + timeout) {
				spdlog::error("{}: {}", what, ec.message());
			}
			else {
				client->open();
			}
		});

		server_handler->set_on_open([this](uint32_t, std::weak_ptr<network::socket_base> socket) {
			if (auto ptr = socket.lock()) {
				ptr->send({ 0,0,0,0 });
			}
			else {
				spdlog::info("could not acquire socket");
				finish(false);
			}
		});
		client_handler->set_on_message([this](std::vector<uint8_t>&& packet) {
			if (packet != std::vector<uint8_t>{ 0,0,0,0 }) {
				spdlog::info("got invalid packet");
				finish(false);
			}
			else {
				finish(true);
			}
		});
	});

	EXPECT_TRUE(wait_for(this));
}

TEST_F(network_test, network_server_receive) {
	auto handle = dispatch_test_task(ioc, [this] {
		server->open();
		client->open();

		client_handler->set_on_error([this](std::string_view what, beast::error_code ec) {
			static const auto start = std::chrono::steady_clock::now();
			static const auto timeout = std::chrono::seconds(2);
			if (std::chrono::steady_clock::now() >= start + timeout) {
				spdlog::error("{}: {}", what, ec.message());
			}
			else {
				client->open();
			}
		});

		server_handler->set_on_message([this](uint32_t, std::vector<uint8_t>&& data) {
			if (data != std::vector<uint8_t>{0, 0, 0, 0}) {
				spdlog::info("got invalid packet");
				finish(false);
			}
			else {
				finish(true);
			}
		});
		client_handler->set_on_open([this]() {
			client->send({ 0,0,0,0 });
		});
	});

	EXPECT_TRUE(wait_for(this));
}
