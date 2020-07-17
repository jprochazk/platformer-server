#pragma once

#include <unordered_set>
#include <pqxx/pqxx>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>

#include <spdlog/spdlog.h>

namespace database
{

	struct settings {
		std::string user;
		std::string password;
		std::string host;
		uint16_t	port;
		std::string name;

		std::string to_string() const {
			return fmt::format("postgresql://{}:{}@{}:{}/{}", user, password, host, port, name);
		}
	};

	using result = pqxx::result;
	using query_handle = boost::future<result>;

	class connection {
	public:
		connection(const settings& settings);
		~connection();

		result query(const std::string& query);
		query_handle async_query(const std::string& query);

		template<typename ...Args>
		result execute(const std::string& name, Args&& ...args);

		template<typename ...Args>
		query_handle async_execute(const std::string& name, Args&& ...args);

		void prepare_statement(const std::string& name, const std::string& query);

		void unprepare_statement(const std::string& name);
	private:
		std::shared_ptr<std::mutex> database_mutex_;
		std::shared_ptr<pqxx::connection> connection_;

		std::unordered_set<std::string> prepared_statements_;
	}; // class connection


	template<typename ...Args>
	result connection::execute(const std::string& name, Args&& ...args) {
		std::lock_guard<std::mutex> lock(*database_mutex_);

		if (prepared_statements_.find(name) == prepared_statements_.end())
			throw std::runtime_error(fmt::format("No prepared statement named \"{}\"", name));

		pqxx::work w(*connection_);
		auto result = w.exec_prepared(name, args...);
		w.commit();

		return result;
	}

	template<typename ...Args>
	query_handle connection::async_execute(const std::string& name, Args&& ...args) {
		std::lock_guard<std::mutex> lock(*database_mutex_);

		if (prepared_statements_.find(name) == prepared_statements_.end())
			throw std::runtime_error(fmt::format("No prepared statement named \"{}\"", name));

		auto m = database_mutex_;
		auto c = connection_;
		return boost::async([m, c, name, args...]{
			std::lock_guard<std::mutex> lock(*m);

			pqxx::work w(*c);
			auto result = w.exec_prepared(name, args...);
			w.commit();

			return result;
		});
	}

}; // namespace database
