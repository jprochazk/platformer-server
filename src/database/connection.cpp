
#include "connection.h"

#include "common/log.h"

namespace database {

connection::connection(const settings& settings)
  : database_mutex_(std::make_shared<std::mutex>())
  , connection_(std::make_shared<pqxx::connection>(settings.to_string()))
  , prepared_statements_()
{
    INFOF("DB::CONN", "New database connection to {}", settings.to_string());
}

connection::~connection()
{
    std::lock_guard<std::mutex> lock(*database_mutex_);

    for (const auto& s : prepared_statements_) {
        connection_->unprepare(s);
    }
}

result
connection::query(const std::string& query)
{
    std::lock_guard<std::mutex> lock(*database_mutex_);

    pqxx::work w(*connection_);
    auto result = w.exec(query);
    w.commit();

    return result;
}

query_handle
connection::async_query(const std::string& query)
{
    auto m = database_mutex_;
    auto c = connection_;
    return boost::async([m, c, query] {
        std::lock_guard<std::mutex> lock(*m);

        pqxx::work w(*c);
        auto result = w.exec(query);
        w.commit();

        return result;
    });
}

void
connection::prepare_statement(const std::string& name, const std::string& query)
{
    std::lock_guard<std::mutex> lock(*database_mutex_);

    if (prepared_statements_.find(name) != prepared_statements_.end()) {
        ERRF("DB::CONN", "Prepared statement \"{}\" already exists", name);
        abort();
    }

    try {
        connection_->prepare(name, query);
        prepared_statements_.insert(name);
    } catch (std::exception e) {
        ERRF("DB::CONN",
             "Failed to prepare statement {{ \"{}\": \"{}\" }}, {}",
             name,
             query,
             e.what());
        abort();
    }
}

void
connection::unprepare_statement(const std::string& name)
{
    std::lock_guard<std::mutex> lock(*database_mutex_);

    if (prepared_statements_.find(name) != prepared_statements_.end())
        return;

    connection_->unprepare(name);
}

}; // namespace database
