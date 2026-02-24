#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& dbPath)
    : databasePath(dbPath), db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open() {
    int rc = sqlite3_open(databasePath.c_str(), &db);

    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::cout << "Database opened successfully!" << std::endl;
    return true;
}

void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool DatabaseManager::createTables(){
    const std::string sql = R"(
    CREATE TABLE IF NOT EXISTS foods (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        barcode TEXT UNIQUE NOT NULL,
        name TEXT NOT NULL,
        calories_per_100g REAL NOT NULL,
        protein REAL,
        carbs REAL,
        fat REAL
        );
    )";

    char* errMsg = nullptr;

    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool DatabaseManager::addFood(const Food& food){
    const char* sql =
        "INSERT INTO foods (barcode, name, calories_per_100g, protein, carbs, fat) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, food.barcode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, food.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, food.calories_per_100g);
    sqlite3_bind_double(stmt, 4, food.protein);
    sqlite3_bind_double(stmt, 5, food.carbs);
    sqlite3_bind_double(stmt, 6, food.fat);

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE){
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::optional<Food> DatabaseManager::getFoodByBarcode(const std::string& barcode) {
    const char* sql =
        "SELECT barcode, name, calories_per_100g, protein, carbs, fat "
        "FROM foods WHERE barcode = ?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, barcode.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        Food food;
        food.barcode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        food.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        food.calories_per_100g = sqlite3_column_double(stmt, 2);
        food.protein = sqlite3_column_double(stmt, 3);
        food.carbs = sqlite3_column_double(stmt, 4);
        food.fat = sqlite3_column_double(stmt, 5);

        sqlite3_finalize(stmt);
        return food;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}