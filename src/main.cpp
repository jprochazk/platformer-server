
#include "network/boost.h"
#include "network/socket_listener.h"
#include "network/socket_handler.h"
#include "database/connection.h"
#include "common/config.h"
#include "common/time.h"
#include "common/log.h"
#include "common/json.h"
#include "game/world.h"
#include "game/system/session.h"
#include "game/system/movement.h"
#include "game/system/sync.h"
#include <entt/entt.hpp>

std::atomic<bool> exitSignal = false;
void signal_handler(boost::system::error_code const& error, int signalNum) 
{
	INFO("SIGNAL_HANDLER", "Received signal {}", (signalNum == SIGINT) ? "SIGINT" : "SIGTERM");
	if(error) {
		ERR("SIGNAL_HANDLER", "Error: {}", error.message());
	}
	exitSignal.store(true, std::memory_order_release);
}

namespace server {
	struct settings {
		std::string ip;
		uint16_t	port;
		int			update_hz;
	};
}

server::settings get_server_settings() {
	auto settings = util::config::get_or_throw<json>("server");

	auto address = settings["address"].get<std::string>();
	auto colon_index = address.find_first_of(":");
	auto ip = address.substr(0, colon_index);
	auto port_str = address.substr(colon_index + 1, address.length() - colon_index);
	auto port = static_cast<uint16_t>(std::atoi(port_str.c_str()));

	auto update_hz = settings["update_hz"].get<int>();

	return {
		ip, port, update_hz
	};
}

database::settings get_db_settings() {
	auto settings = util::config::get_or_throw<json>("database");
	auto user = settings["user"].get<std::string>();
	auto password = settings["password"].get<std::string>();
	auto host = settings["host"].get<std::string>();
	auto port = settings["port"].get<uint16_t>();
	auto name = settings["name"].get<std::string>();

	return database::settings{ user,password,host,port,name };
}

json get_map_data() {
	return load_json_file("map.json");
}

std::shared_ptr<std::thread> start_network(
	net::io_context& ioc, 
	std::shared_ptr<network::socket_handler> socket_handler,
	tcp::endpoint endpoint) 
{
	return { 
		new std::thread(
		[&ioc, socket_handler, endpoint] {
			INFO("NETWORK", "Starting network thread");

			net::signal_set signals(ioc, SIGINT, SIGTERM);
			signals.async_wait(signal_handler);

			// Create and launch a listening port
			std::make_shared<network::socket_listener>(ioc, endpoint, socket_handler)->open();

			// run the I/O service
			ioc.run();
		}),
		[&ioc](std::thread* thread) {
			// custom deleter which also stops the io context
			ioc.stop();
			thread->join();
			delete thread;
		} 
	};
}

std::shared_ptr<game::world> init_world() {
	auto world = std::make_shared<game::world>(get_db_settings(), get_map_data());
	world->add_system("session", std::make_shared<game::system::session>(world));
	world->add_system("movement", std::make_shared<game::system::movement>(world));
	world->add_system("sync", std::make_shared<game::system::sync>(world));
	return world;
}

int main()
{
	util::config::load("config.json");

	auto server_settings = get_server_settings();
	INFO("MAIN", "Starting server at {}:{}", server_settings.ip, server_settings.port);

	auto world = init_world();

	net::io_context ioc;
	auto networkHandle = start_network(
		ioc, world->get_system<game::system::session>("session"), // socket handler
		tcp::endpoint{ net::ip::make_address(server_settings.ip), server_settings.port }
	);

	INFO("MAIN", "Starting game loop");
	auto last_tick = util::time::now();
	while (!exitSignal.load(std::memory_order_acquire)) {
		auto now = util::time::now();
		auto time_since_tick = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count();
		if (time_since_tick >= 1000.f / server_settings.update_hz) {
			last_tick = now;

			world->update();
		}
	}

	return EXIT_SUCCESS;
}
