#include "database.h"
#include <stdexcept>

std::unique_ptr<DatabaseConnection> DatabaseConnection::instance_ = nullptr;

DatabaseConnection::DatabaseConnection(const std::string& host,
                                     const std::string& user,
                                     const std::string& password,
                                     const std::string& database,
                                     unsigned int port)
    : host_(host)
    , user_(user)
    , password_(password)
    , database_(database)
    , port_(port)
    , connection_(nullptr) {
    connection_ = mysql_init(nullptr);
    if (!connection_) {
        throw std::runtime_error("Failed to initialize MySQL connection object");
    }
}

DatabaseConnection::~DatabaseConnection() {
    if (connection_) {
        mysql_close(connection_);
    }
}

void DatabaseConnection::initialize(const std::string& host,
                                  const std::string& user,
                                  const std::string& password,
                                  const std::string& database,
                                  unsigned int port) {
    if (!instance_) {
        instance_.reset(new DatabaseConnection(host, user, password, database, port));
    }
}

DatabaseConnection& DatabaseConnection::getInstance() {
    if (!instance_) {
        throw std::runtime_error("Database not initialized. Call initialize() first.");
    }
    return *instance_;
}

bool DatabaseConnection::connect() {
    if (!mysql_real_connect(connection_, 
                          host_.c_str(),
                          user_.c_str(),
                          password_.c_str(),
                          database_.c_str(),
                          port_,
                          nullptr,
                          0)) {
        return false;
    }
    return true;
}

void DatabaseConnection::disconnect() {
    if (connection_) {
        mysql_close(connection_);
        connection_ = mysql_init(nullptr);
    }
}

bool DatabaseConnection::isConnected() const {
    return mysql_ping(connection_) == 0;
}

std::string DatabaseConnection::getCurrentDatabase() const {
    const char* db = mysql_get_server_info(connection_);
    return db ? db : "";
}

std::string DatabaseConnection::getServerInfo() const {
    const char* info = mysql_get_server_info(connection_);
    return info ? info : "";
}

std::string DatabaseConnection::getClientInfo() const {
    const char* info = mysql_get_client_info();
    return info ? info : "";
}

std::string DatabaseConnection::getLastError() const {
    const char* error = mysql_error(connection_);
    return error ? error : "";
}