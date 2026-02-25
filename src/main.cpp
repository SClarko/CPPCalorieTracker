#include <iostream>
#include <string>
#include <limits>
#include "DatabaseManager.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cctype>

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string todayDateISO() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

    std::tm localTm{};
    localtime_s(&localTm, &t);

    std::ostringstream oss;
    oss << std::put_time(&localTm, "%Y-%m-%d");
    return oss.str();
}

static bool isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int daysInMonth(int y, int m) {
    static const int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (m == 2) return isLeapYear(y) ? 29 : 28;
    return days[m - 1];
}

static bool isValidISODate(const std::string& s) {
    // Format: YYYY-MM-DD (length 10, digits and dashes in right spots)
    if (s.size() != 10) return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) {
            if (s[i] != '-') return false;
        } else {
            if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
        }
    }

    int y = std::stoi(s.substr(0, 4));
    int m = std::stoi(s.substr(5, 2));
    int d = std::stoi(s.substr(8, 2));

    if (y < 1900 || y > 2100) return false;   // reasonable bounds
    if (m < 1 || m > 12) return false;

    int dim = daysInMonth(y, m);
    if (d < 1 || d > dim) return false;

    return true;
}

static std::string chooseDateOrToday() {
    const std::string today = todayDateISO();

    while (true) {
        std::cout << "Date (YYYY-MM-DD) [press Enter for " << today << "]: ";
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) return today;

        if (isValidISODate(input)) return input;

        std::cout << "Invalid date. Please use YYYY-MM-DD (e.g. 2026-02-24).\n";
    }
}

int main() {
    DatabaseManager db("../data/calories.db");

    if (!db.open()) return 1;
    if (!db.createTables()) return 1;

    std::cout << "\n1) Add food\n2) Lookup food by barcode\n3) Log food eaten\n4) Show total calories for a date\n5) Set daily calorie goal\n9) Admin\nChoose: ";
    int choice = 0;
    std::cin >> choice;

    if (!std::cin) {
        clearInput();
        std::cout << "Invalid input.\n";
        return 0;
    }

    clearInput();

    if (choice == 1) {
        Food f{};
        std::cout << "Barcode: ";
        std::cin >> f.barcode;

        clearInput();
        std::cout << "Name: ";
        std::getline(std::cin, f.name);

        std::cout << "Calories per 100g: ";
        std::cin >> f.calories_per_100g;
        std::cout << "Protein per 100g: ";
        std::cin >> f.protein;
        std::cout << "Carbs per 100g: ";
        std::cin >> f.carbs;
        std::cout << "Fat per 100g: ";
        std::cin >> f.fat;

        if (db.addFood(f)) {
            std::cout << "Food added.\n";
        } else {
            std::cout << "Failed to add food (maybe barcode already exists).\n";
        }
    }
    else if (choice == 2) {
        std::string barcode;
        std::cout << "Enter barcode: ";
        std::cin >> barcode;

        auto food = db.getFoodByBarcode(barcode);
        if (!food) {
            std::cout << "Not found.\n";
            return 0;
        }

        std::cout << "\nFound: " << food->name << "\n";
        std::cout << "Per 100g: " << food->calories_per_100g << " kcal | "
                  << "P " << food->protein << "g, "
                  << "C " << food->carbs << "g, "
                  << "F " << food->fat << "g\n";

        double grams = 0;
        std::cout << "Enter grams eaten: ";
        std::cin >> grams;

        double calories = (food->calories_per_100g / 100.0) * grams;
        std::cout << "Calories for " << grams << "g: " << calories << " kcal\n";
    }
    else if (choice == 3){
        std::string barcode;
        std::string date = chooseDateOrToday();
        double grams = 0;

        std::cout << "Using today's date: " << date << "\n";

        std::cout << "Barcode: ";
        std::cin >> barcode;

        std::cout << "Grams eaten: ";
        std::cin >> grams;

        if (db.logFoodForDate(date, barcode, grams)) {
            std::cout << "Logged.\n";
        } else {
            std::cout << "Failed to log.\n";
        }
    }
    else if (choice == 4) {
        std::string date = chooseDateOrToday();
        std::cout << "Showing today's entries (" << date << ")\n";
        
        auto entries = db.getEntriesForDate(date);

        if(entries.empty()) {
            std::cout << "No entries for " << date << ".\n";
            return 0;
        }

        std::cout << "\nEntries for " << date << ":\n";
        for (const auto& e : entries) {
            std::cout << "- " << e.name << " (" << e.barcode << ") " << e.grams << "g -> " << e.calories << " kcal\n";
        }

        double total = db.getTotalCaloriesForDate(date);
        std::cout << "Total: " << total << " kcal\n";

        double goal = db.getDailyGoal();

        if (goal > 0) {
            double remaining = goal - total;

            std::cout << "Goal: " << goal << " kcal\n";
            std::cout << "Consumed: " << total << " kcal\n";

            if (remaining > 0) {
                std::cout << "Remaining: " << remaining << " kcal\n";
            }
            else if (remaining > 0) {
                std::cout << "Over by: " << -remaining << " kcal\n";
            }
            else {
                std::cout << "Exactly on target.\n";
            }
        }
    }
    else if (choice == 5) {
        double goal;
        std::cout << "Enter daily calorie goal: ";
        std::cin >> goal;

        if (db.setDailyGoal(goal)) {
            std::cout << "Goal set.\n";
        } else {
            std::cout << "Failed to set goal.\n";
        }
    }
    else if (choice == 9) {
        std::string code;
        std::cout << "Admin code: ";
        std::getline(std::cin, code);

        if (code != "1234") {  // change this to something you like
            std::cout << "Access denied.\n";
            return 0;
        }

        std::cout << "\nADMIN MENU\n";
        std::cout << "1) Clear all daily logs\n";
        std::cout << "2) Clear all foods (also clears logs)\n";
        std::cout << "3) Factory reset (wipe everything)\n";
        std::cout << "Choose: ";

        int adminChoice = 0;
        std::cin >> adminChoice;
        if (!std::cin) { clearInput(); std::cout << "Invalid input.\n"; return 0; }
        clearInput();

        // extra safety confirmation
        std::string confirm;
        std::cout << "Type DELETE to confirm: ";
        std::getline(std::cin, confirm);

        if (confirm != "DELETE") {
            std::cout << "Cancelled.\n";
            return 0;
        }

        bool ok = false;
        if (adminChoice == 1) ok = db.clearAllLogs();
        else if (adminChoice == 2) ok = db.clearAllFoods();
        else if (adminChoice == 3) ok = db.factoryReset();
        else { std::cout << "Unknown option.\n"; return 0; }

        std::cout << (ok ? "Done.\n" : "Operation failed.\n");
    }
    else {
        std::cout << "Unknown option.\n";
    }

    return 0;
}