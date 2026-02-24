#pragma once
#include <string>
#include "sqlite3.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    bool open();
    void close();

private:
    std::string databasePath;
    sqlite3* db;
};