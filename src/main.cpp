#include <iostream>
#include <string>
#include <limits>
#include "DatabaseManager.h"

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    DatabaseManager db("../data/calories.db");

    if (!db.open()) return 1;
    if (!db.createTables()) return 1;

    std::cout << "\n1) Add food\n2) Lookup food by barcode\n3) Log food eaten\n4) Show total calories for a date\nChoose: ";
    int choice = 0;
    std::cin >> choice;

    if (!std::cin) {
        clearInput();
        std::cout << "Invalid input.\n";
        return 0;
    }

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
        std::string date, barcode;
        double grams = 0;

        std::cout << "Date (YYYY-MM-DD): ";
        std::cin >> date;

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
        std::string date;
        std::cout << "Date (YYYY-MM-DD): ";
        std::cin >> date;

        double total = db.getTotalCaloriesForDate(date);
        std::cout << "Total calories for " << date << ": " << total << " kcal\n";
    }
    else {
        std::cout << "Unknown option.\n";
    }

    return 0;
}