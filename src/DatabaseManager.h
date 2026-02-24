#pragma once
#include <string>
#include "sqlite3.h"
#include <optional>

struct Food {
    std::string barcode;
    std::string name;
    double calories_per_100g;
    double protein;
    double carbs;
    double fat;
};

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    bool open();
    void close();
    bool createTables();
    bool logFoodForDate(const std::string& date, const std::string& barcode, double grams);
    double getTotalCaloriesForDate(const std::string& date);

bool addFood(const Food& food);
std::optional<Food> getFoodByBarcode(const std::string& barcode);

private:
    std::string databasePath;
    sqlite3* db;
};