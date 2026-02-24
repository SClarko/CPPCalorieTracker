#include <iostream>
#include "DatabaseManager.h"

int main() {
    DatabaseManager db ("../data/calories.db");

    if (!db.open()) {
        return 1;
    }

    if (!db.createTables()) {
        return 1;
    }

    std::cout << "Database initialised successfully.\n";
    
    return 0;
}