// CAP455 - Minor Project - CA2 (Persist & Search)
// Project : Earthquake Shelter Occupancy Manager
// Scope   : CA1 (array of objects, pointers, static/inline/friend, validation)
//           + constructors, destructor, binary file storage (fixed-size
//           records), search with seekg, basic update with seekp.
// Run this program from the project root so that data/shelters.dat is found.

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <limits>
using namespace std;

const int MAX_SHELTERS = 50;           // size of the array of objects
const char* DATA_FILE = "data/shelters.dat";
const int NAME_LEN = 30;               // fixed sizes so every record has equal size
const int AREA_LEN = 30;

// ---------------------------------------------------------------
// Fixed-size record that is written to / read from the binary file.
// (std::string cannot be written directly, so char arrays are used.)
// ---------------------------------------------------------------
struct ShelterRecord {
    int  id;
    char name[NAME_LEN];
    char area[AREA_LEN];
    int  capacity;
    int  occupied;
};

// ---------------------------------------------------------------
// Entity class: Shelter
// ---------------------------------------------------------------
class Shelter {
private:
    int    shelterId;                  // unique key
    string name;
    string area;
    int    capacity;
    int    occupied;

    static int  totalShelters;         // number of shelters stored (from CA1)

public:
    // Default constructor
    Shelter() : shelterId(0), name(""), area(""), capacity(0), occupied(0) {}

    // Parameterised constructor (uses an initializer list)
    Shelter(int id, const string& n, const string& a, int c, int o)
        : shelterId(id), name(n), area(a), capacity(c), occupied(o) {}

    // Copy constructor (used in updateOccupancy() to keep a backup)
    Shelter(const Shelter& other)
        : shelterId(other.shelterId), name(other.name), area(other.area),
          capacity(other.capacity), occupied(other.occupied) {}

    // Destructor: runs automatically when an object ends.
    // Nothing to release yet (no dynamic memory); CA3 will free memory here.
    ~Shelter() {}

    // setters (use 'this', return reference so calls can be chained)
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

    string getStatus() const {
        if (occupied >= capacity)            return "FULL";
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

    // Convert object -> record (for writing to file)
    ShelterRecord toRecord() const {
        ShelterRecord r;
        memset(&r, 0, sizeof(r));
        r.id = shelterId;
        strncpy(r.name, name.c_str(), NAME_LEN - 1);
        strncpy(r.area, area.c_str(), AREA_LEN - 1);
        r.capacity = capacity;
        r.occupied = occupied;
        return r;
    }

    // Convert record -> object (after reading from file)
    void fromRecord(const ShelterRecord& r) {
        shelterId = r.id;
        name      = r.name;
        area      = r.area;
        capacity  = r.capacity;
        occupied  = r.occupied;
    }

    static void increaseCount()            { totalShelters++; }
    static int  getTotalShelters()         { return totalShelters; }

    friend void printOccupancyReport(const Shelter& s);
};

int  Shelter::totalShelters = 0;

void printOccupancyReport(const Shelter& s) {
    double percent = (s.capacity > 0) ? (s.occupied * 100.0 / s.capacity) : 0;
    cout << "\n--- Occupancy Report ---\n";
    cout << "Shelter  : " << s.name << " (" << s.area << ")\n";
    cout << "Occupied : " << s.occupied << " / " << s.capacity
         << "  (" << percent << "%)\n";
    cout << "Status   : " << s.getStatus() << "\n";
}

// ---------------------------------------------------------------
// Global storage: array of objects (a working copy of the file)
// ---------------------------------------------------------------
Shelter shelters[MAX_SHELTERS];

// ---------------------------------------------------------------
// Input helpers
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

string readText(const string& prompt, int maxLen) {
    string text;
    while (true) {
        cout << prompt;
        getline(cin, text);
        if (text.empty())
            cout << "Text cannot be empty.\n";
        else if ((int)text.length() > maxLen)
            cout << "Too long. Maximum " << maxLen << " characters.\n";
        else
            return text;
    }
}

// ---------------------------------------------------------------
// File helper functions
// ---------------------------------------------------------------

// Number of records in the file = file size / size of one record
int countRecords() {
    ifstream file(DATA_FILE, ios::binary);
    if (!file) return 0;
    file.seekg(0, ios::end);
    return (int)(file.tellg() / sizeof(ShelterRecord));
}

// Searches record by record using seekg.
// Returns the record number (0, 1, 2 ...) or -1 if not found.
int findIndexById(int id, ShelterRecord& found) {
    ifstream file(DATA_FILE, ios::binary);
    if (!file) return -1;

    int total = countRecords();
    for (int i = 0; i < total; i++) {
        file.seekg(static_cast<streamoff>(i) * sizeof(ShelterRecord), ios::beg);
        file.read(reinterpret_cast<char*>(&found), sizeof(found));
        if (found.id == id)
            return i;
    }
    return -1;
}

// Returns a pointer to the matching object in the array, or nullptr
Shelter* findShelter(int id) {
    for (int i = 0; i < Shelter::getTotalShelters(); i++) {
        if (shelters[i].getId() == id)
            return &shelters[i];
    }
    return nullptr;
}

// Reads every record from the file into the array when the program starts.
// The array keeps the same order as the file, so array index = record number - 1.
void loadShelters() {
    ifstream file(DATA_FILE, ios::binary);
    if (!file) return;                       // no file yet: start empty

    ShelterRecord rec;
    while (Shelter::getTotalShelters() < MAX_SHELTERS &&
           file.read(reinterpret_cast<char*>(&rec), sizeof(rec))) {
        shelters[Shelter::getTotalShelters()].fromRecord(rec);
        Shelter::increaseCount();
    }
    if (countRecords() > MAX_SHELTERS)
        cout << "Warning: file has more than " << MAX_SHELTERS << " records; only the first "
             << MAX_SHELTERS << " were loaded.\n";
}

// ---------------------------------------------------------------
// CREATE (kept in the array and saved to the file)
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
    if (findShelter(id) != nullptr) {          // pointer used for duplicate check
        cout << "A shelter with this ID already exists.\n";
        return;
    }

    string name = readText("Enter shelter name: ", NAME_LEN - 1);
    string area = readText("Enter area/location: ", AREA_LEN - 1);

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

    Shelter s(id, name, area, capacity, occupied);   // parameterised constructor
    ShelterRecord rec = s.toRecord();

    ofstream file(DATA_FILE, ios::binary | ios::app);
    if (!file) {
        cout << "Could not open data file. Run the program from the project root (data/ folder needed).\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(&rec), sizeof(rec));

    shelters[Shelter::getTotalShelters()].fromRecord(rec);   // keep the array in step with the file
    Shelter::increaseCount();
    cout << "Shelter saved to file.\n";
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

// Search by ID (sequential search using seekg)
void searchShelterById() {
    int id = readInt("Enter Shelter ID to search: ");
    ShelterRecord rec;
    int index = findIndexById(id, rec);
    if (index == -1) {
        cout << "Shelter not found.\n";
        return;
    }
    Shelter s;
    s.fromRecord(rec);
    cout << "Found at record number " << (index + 1) << ":\n";
    s.display();
    printOccupancyReport(s);
}

// Random access: jump straight to record N using seekg
void viewByRecordNumber() {
    int total = countRecords();
    if (total == 0) {
        cout << "No shelters recorded yet.\n";
        return;
    }
    int n = readInt("Enter record number (1 to " + to_string(total) + "): ");
    if (n < 1 || n > total) {
        cout << "Record number out of range.\n";
        return;
    }

    ifstream file(DATA_FILE, ios::binary);
    file.seekg(static_cast<streamoff>(n - 1) * sizeof(ShelterRecord), ios::beg);
    ShelterRecord rec;
    file.read(reinterpret_cast<char*>(&rec), sizeof(rec));

    Shelter s;
    s.fromRecord(rec);
    s.display();
}

// ---------------------------------------------------------------
// BASIC UPDATE: change the number of people inside (uses seekp)
// ---------------------------------------------------------------
void updateOccupancy() {
    int id = readInt("Enter Shelter ID to update: ");
    Shelter* ptr = findShelter(id);            // pointer to the object in the array
    if (ptr == nullptr) {
        cout << "Shelter not found.\n";
        return;
    }

    cout << "Current record:\n";
    ptr->display();

    int newOccupied = readInt("Enter new number of people inside: ");
    if (newOccupied < 0 || newOccupied > ptr->getCapacity()) {
        cout << "Occupied must be between 0 and capacity. Update cancelled.\n";
        return;
    }

    Shelter backup(*ptr);                      // copy constructor: keep the old values
    ptr->setOccupied(newOccupied);             // change the object in memory

    int index = ptr - shelters;                // pointer arithmetic: record number - 1
    ShelterRecord rec = ptr->toRecord();

    fstream file(DATA_FILE, ios::in | ios::out | ios::binary);
    if (!file) {                               // file missing: undo the change in memory
        ptr->setOccupied(backup.getOccupied());
        cout << "Could not open data file. Update undone.\n";
        return;
    }
    file.seekp(static_cast<streamoff>(index) * sizeof(ShelterRecord), ios::beg);
    file.write(reinterpret_cast<const char*>(&rec), sizeof(rec));

    cout << "Record updated.\n";
    ptr->display();
}

// ---------------------------------------------------------------
// MAIN MENU
// ---------------------------------------------------------------
int main() {
    loadShelters();                            // read the file into the array

    int choice;
    do {
        cout << "\n===== Earthquake Shelter Occupancy Manager =====\n";
        cout << "1. Add new shelter\n";
        cout << "2. View all shelters\n";
        cout << "3. Search shelter by ID\n";
        cout << "4. View shelter by record number\n";
        cout << "5. Update people inside a shelter\n";
        cout << "0. Exit\n";
        choice = readInt("Enter choice: ");

        switch (choice) {
            case 1: addShelter();         break;
            case 2: viewAllShelters();    break;
            case 3: searchShelterById();  break;
            case 4: viewByRecordNumber(); break;
            case 5: updateOccupancy();    break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
