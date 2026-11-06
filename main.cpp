// CAP455 - Minor Project - CA1 (Design & Create)
// Project : Earthquake Shelter Occupancy Manager
// Scope   : In-memory Create & Read with validation, no duplicate IDs.

#include <iostream>
#include <string>
#include <limits>
using namespace std;

const int MAX_SHELTERS = 50;           // size of the array of objects

// ---------------------------------------------------------------
// Entity class: Shelter
// ---------------------------------------------------------------
class Shelter {
private:
    int    shelterId;                  // unique key
    string name;
    string area;
    int    capacity;                   // max people allowed
    int    occupied;                   // people currently inside

    static int totalShelters;          // static data member

public:
    Shelter() : shelterId(0), name(""), area(""), capacity(0), occupied(0) {}

    // setters (use 'this' pointer, return reference so calls can be chained)
    Shelter& setId(int id)            { this->shelterId = id;  return *this; }
    Shelter& setName(const string& n) { this->name = n;        return *this; }
    Shelter& setArea(const string& a) { this->area = a;        return *this; }
    Shelter& setCapacity(int c)       { this->capacity = c;    return *this; }
    Shelter& setOccupied(int o)       { this->occupied = o;    return *this; }

    // inline getters
    inline int    getId() const       { return shelterId; }
    inline string getName() const     { return name; }
    inline string getArea() const     { return area; }
    inline int    getCapacity() const { return capacity; }
    inline int    getOccupied() const { return occupied; }
    inline int    getVacancy() const  { return capacity - occupied; }

    // status text based on how full the shelter is
    string getStatus() const {
        if (occupied >= capacity)          return "FULL";
        if (occupied * 100 >= capacity * 80) return "ALMOST FULL";
        return "AVAILABLE";
    }

    void display() const {
        cout << "ID: " << shelterId
             << " | " << name
             << " | Area: " << area
             << " | Capacity: " << capacity
             << " | Occupied: " << occupied
             << " | Vacancy: " << getVacancy()
             << " | " << getStatus() << endl;
    }

    // static member functions
    static void increaseCount()  { totalShelters++; }
    static int  getTotalShelters() { return totalShelters; }

    // friend function: can read private data directly
    friend void printOccupancyReport(const Shelter& s);
};

int Shelter::totalShelters = 0;        // definition of static member

// Friend function definition
void printOccupancyReport(const Shelter& s) {
    double percent = (s.capacity > 0) ? (s.occupied * 100.0 / s.capacity) : 0;
    cout << "\n--- Occupancy Report ---\n";
    cout << "Shelter  : " << s.name << " (" << s.area << ")\n";
    cout << "Occupied : " << s.occupied << " / " << s.capacity
         << "  (" << percent << "%)\n";
    cout << "Status   : " << s.getStatus() << "\n";
}

// ---------------------------------------------------------------
// Global storage: array of objects
// ---------------------------------------------------------------
Shelter shelters[MAX_SHELTERS];

// ---------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------
int readInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Invalid input. Enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

string readText(const string& prompt) {
    string text;
    do {
        cout << prompt;
        getline(cin, text);
        if (text.empty()) cout << "Text cannot be empty.\n";
    } while (text.empty());
    return text;
}

// Returns a pointer to the matching shelter, or nullptr if not found
Shelter* findShelter(int id) {
    for (int i = 0; i < Shelter::getTotalShelters(); i++) {
        if (shelters[i].getId() == id)
            return &shelters[i];
    }
    return nullptr;
}

// ---------------------------------------------------------------
// CREATE
// ---------------------------------------------------------------
void addShelter() {
    if (Shelter::getTotalShelters() >= MAX_SHELTERS) {
        cout << "Storage full. Cannot add more shelters.\n";
        return;
    }

    int id = readInt("Enter Shelter ID (positive number): ");
    if (id <= 0) {
        cout << "ID must be greater than 0.\n";
        return;
    }
    if (findShelter(id) != nullptr) {
        cout << "A shelter with this ID already exists.\n";
        return;
    }

    string name = readText("Enter shelter name: ");
    string area = readText("Enter area/location: ");

    int capacity = readInt("Enter capacity (1 - 5000): ");
    if (capacity < 1 || capacity > 5000) {
        cout << "Capacity must be between 1 and 5000.\n";
        return;
    }

    int occupied = readInt("Enter people currently inside: ");
    if (occupied < 0 || occupied > capacity) {
        cout << "Occupied must be between 0 and capacity.\n";
        return;
    }

    int index = Shelter::getTotalShelters();
    shelters[index].setId(id).setName(name).setArea(area)
                   .setCapacity(capacity).setOccupied(occupied);
    Shelter::increaseCount();

    cout << "Shelter added successfully.\n";
}

// ---------------------------------------------------------------
// READ
// ---------------------------------------------------------------
void viewAllShelters() {
    if (Shelter::getTotalShelters() == 0) {
        cout << "No shelters recorded yet.\n";
        return;
    }
    cout << "\n--- All Shelters ---\n";
    for (int i = 0; i < Shelter::getTotalShelters(); i++)
        shelters[i].display();
    cout << "Total shelters: " << Shelter::getTotalShelters() << endl;
}

void viewShelterById() {
    int id = readInt("Enter Shelter ID to search: ");
    Shelter* ptr = findShelter(id);
    if (ptr == nullptr) {
        cout << "Shelter not found.\n";
        return;
    }
    ptr->display();
    printOccupancyReport(*ptr);
}

// ---------------------------------------------------------------
// MAIN MENU
// ---------------------------------------------------------------
int main() {
    int choice;
    do {
        cout << "\n===== Earthquake Shelter Occupancy Manager =====\n";
        cout << "1. Add new shelter\n";
        cout << "2. View all shelters\n";
        cout << "3. Search shelter by ID\n";
        cout << "0. Exit\n";
        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1: addShelter();       break;
            case 2: viewAllShelters();  break;
            case 3: viewShelterById();  break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
