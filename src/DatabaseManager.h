#pragma once
#include <string>
#include "sqlite3.h"
#include <optional>
#include <vector>

struct Food {
    std::string barcode;
    std::string name;
    double calories_per_100g;
    double protein;
    double carbs;
    double fat;
};

struct LogEntry{
    std::string name;
    std::string barcode;
    double grams;
    double calories;
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
    std::vector<LogEntry> getEntriesForDate(const std::string& date);
    bool clearAllLogs();
    bool clearAllFoods();
    bool factoryReset();
    bool setDailyGoal(double goal);
    double getDailyGoal();

private:
    std::string databasePath;
    sqlite3* db;
};