#include <iostream>
#include "sqlite3.h"

class DepartmentDatabase {
private:
    sqlite3* db;

public:
    // Constructor
    DepartmentDatabase() : db(nullptr) {
        // Open the database
        if (sqlite3_open("personal.db", &db) != SQLITE_OK) {
            std::cerr << "Error opening the database." << std::endl;
            throw std::runtime_error("Failed to open the database.");
        }

        // Create the 'department' table if it does not exist
        const char* createDepartmentTableQuery =
            "CREATE TABLE IF NOT EXISTS department ("
            "code INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "location TEXT NOT NULL"
            ");";

        if (sqlite3_exec(db, createDepartmentTableQuery, nullptr, nullptr, nullptr) != SQLITE_OK) {
            std::cerr << "Error creating the table in the database." << std::endl;
            throw std::runtime_error("Failed to create the 'department' table.");
        }
    }

    ~DepartmentDatabase() {
        sqlite3_close(db);
    }

    // Insert a department into the database
    void insertDepartment(const std::string& name, const std::string& location) {
        const char* insertQuery = "INSERT INTO department (name, location) VALUES (?, ?);";

        sqlite3_stmt* statement;

        if (sqlite3_prepare_v2(db, insertQuery, -1, &statement, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(statement, 2, location.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(statement) == SQLITE_DONE) {
                std::cout << "A department has been inserted into the database." << std::endl;
            } else {
                std::cerr << "Error inserting the department." << std::endl;
            }

            sqlite3_finalize(statement);
        } else {
            std::cerr << "Error preparing the SQL statement." << std::endl;
        }
    }

    // Retrieve all departments from the database
    void retrieveAllDepartments() {
        const char* selectQuery = "SELECT * FROM department;";

        sqlite3_stmt* statement;

        if (sqlite3_prepare_v2(db, selectQuery, -1, &statement, nullptr) == SQLITE_OK) {
            int count = 0;

            while (sqlite3_step(statement) == SQLITE_ROW) {
                int code = sqlite3_column_int(statement, 0);
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
                const char* location = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));

                std::cout << "Department [Code = " << code << ", Name = " << name
                          << ", Location = " << location << "]" << std::endl;

                count++;
            }

            if (count == 0) {
                std::cout << "No departments found in the database." << std::endl;
            } else {
                std::cout << "Queried " << count << " departments from the database." << std::endl;
            }

            sqlite3_finalize(statement);
        } else {
            std::cerr << "Error preparing the SQL statement." << std::endl;
        }
    }

    // Retrieve a department by code from the database
    void retrieveDepartmentByCode(int code) {
        const char* selectQuery = "SELECT * FROM department WHERE code = ?;";

        sqlite3_stmt* statement;

        if (sqlite3_prepare_v2(db, selectQuery, -1, &statement, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(statement, 1, code);

            if (sqlite3_step(statement) == SQLITE_ROW) {
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
                const char* location = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));

                std::cout << "Department [Code = " << code << ", Name = " << name
                          << ", Location = " << location << "]" << std::endl;
            } else {
                std::cout << "No department with that code found in the database." << std::endl;
            }

            sqlite3_finalize(statement);
        } else {
            std::cerr << "Error preparing the SQL statement." << std::endl;
        }
    }

    // Update a department by code in the database
    void updateDepartment(int code, const std::string& newName, const std::string& newLocation) {
        const char* updateQuery = "UPDATE department SET name = ?, location = ? WHERE code = ?;";

        sqlite3_stmt* statement;

        if (sqlite3_prepare_v2(db, updateQuery, -1, &statement, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(statement, 1, newName.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(statement, 2, newLocation.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(statement, 3, code);

            if (sqlite3_step(statement) == SQLITE_DONE) {
                std::cout << "A department in the database has been updated." << std::endl;
            } else {
                std::cout << "No department with that code found in the database." << std::endl;
            }

            sqlite3_finalize(statement);
        } else {
            std::cerr << "Error preparing the SQL statement." << std::endl;
        }
    }

    // Delete a department by code from the database
    void deleteDepartment(int code) {
        const char* deleteQuery = "DELETE FROM department WHERE code = ?;";

        sqlite3_stmt* statement;

        if (sqlite3_prepare_v2(db, deleteQuery, -1, &statement, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(statement, 1, code);

            int result = sqlite3_step(statement);

            if (result == SQLITE_DONE) {
                std::cout << "A department has been deleted from the database." << std::endl;
            } else if (result == SQLITE_CONSTRAINT) {
                std::cout << "The department is referenced in one or more employees in the database." << std::endl;
            } else {
                std::cout << "No department with that code found in the database." << std::endl;
            }

            sqlite3_finalize(statement);
        } else {
            std::cerr << "Error preparing the SQL statement." << std::endl;
        }
    }
};

int main() {
    DepartmentDatabase departmentDatabase;

    int option;
    do {
        std::cout << "Menu options:" << std::endl;
        std::cout << "0) Exit the program." << std::endl;
        std::cout << "1) Insert a department into the database." << std::endl;
        std::cout << "2) Retrieve all departments from the database." << std::endl;
        std::cout << "3) Retrieve a department by code from the database." << std::endl;
        std::cout << "4) Update a department by code in the database." << std::endl;
        std::cout << "5) Delete a department by code from the database." << std::endl;

        std::cout << "Enter an option: ";
        std::cin >> option;

        switch (option) {
            case 0:
                std::cout << "Exiting the program." << std::endl;
                break;

            case 1:
                {
                    std::string name, location;
                    std::cout << "Enter the name of the department: ";
                    std::cin >> name;
                    std::cout << "Enter the location of the department: ";
                    std::cin >> location;

                    departmentDatabase.insertDepartment(name, location);
                }
                break;

            case 2:
                departmentDatabase.retrieveAllDepartments();
                break;

            case 3:
                {
                    int code;
                    std::cout << "Enter the code of the department: ";
                    std::cin >> code;

                    departmentDatabase.retrieveDepartmentByCode(code);
                }
                break;

            case 4:
                {
                    int code;
                    std::string newName, newLocation;
                    std::cout << "Enter the code of the department to update: ";
                    std::cin >> code;
                    std::cout << "Enter the new name of the department: ";
                    std::cin >> newName;
                    std::cout << "Enter the new location of the department: ";
                    std::cin >> newLocation;

                    departmentDatabase.updateDepartment(code, newName, newLocation);
                }
                break;

            case 5:
                {
                    int code;
                    std::cout << "Enter the code of the department to delete: ";
                    std::cin >> code;

                    departmentDatabase.deleteDepartment(code);
                }
                break;

            default:
                std::cerr << "Menu option must be between 0 and 5." << std::endl;
        }

    } while (option != 0);

    return 0;
}
