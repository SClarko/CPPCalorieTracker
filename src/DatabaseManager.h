#pragma once
#include <string>
#include "sqlite3.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    bool open();
    void close();
    bool createTables();

private:
    std::string databasePath;
    sqlite3* db;
};