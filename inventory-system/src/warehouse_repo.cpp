// warehouse_repo.cpp
#include "warehouse_repo.h"
#include "database_connection.h"
#include <soci/soci.h>

WarehouseRepository::WarehouseRepository() {}

WarehouseRepository::~WarehouseRepository() {}

bool WarehouseRepository::create(const Warehouse& w) {
    try {
        soci::session& sql = DatabaseConnection::getInstance().getSession();
        sql << "INSERT INTO warehouses (code, name, location, status) "
            "VALUES (:code, :name, :location, :status)",
            soci::use(w.code),
            soci::use(w.name),
            soci::use(w.location),
            soci::use(w.status);
        return true;
    } catch (const std::exception& e) {
        // 實際應用中應該要有錯誤處理機制
        return false;
    }
}

Warehouse WarehouseRepository::findWarehouseByCode(const std::string& code) {
    Warehouse w;
    try {
        soci::session& sql = DatabaseConnection::getInstance().getSession();
        sql << "SELECT code, name, location, status "
            "FROM warehouses WHERE code = :code",
            soci::use(code),
            soci::into(w.code),
            soci::into(w.name),
            soci::into(w.location),
            soci::into(w.status);
    } catch (const std::exception& e) {
        // 實際應用中應該要有錯誤處理機制
    }
    return w;
}

bool WarehouseRepository::update(const Warehouse& w) {
    try {
        soci::session& sql = DatabaseConnection::getInstance().getSession();
        sql << "UPDATE warehouses "
            "SET name = :name, location = :location, status = :status "
            "WHERE code = :code",
            soci::use(w.name),
            soci::use(w.location),
            soci::use(w.status),
            soci::use(w.code);
        return true;
    } catch (const std::exception& e) {
        // 實際應用中應該要有錯誤處理機制
        return false;
    }
}