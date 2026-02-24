#include <iostream>
#include "DatabaseManager.h"

int main() {
    DatabaseManager db ("../data/calories.db");

    if (!db.open()) {
        return 1;
    }

    std::cout << "Setup complete.\n";
    
    return 0;
}