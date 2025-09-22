#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

// Define a structure to hold order data
struct Order {
    int quantities[9];
    double total;
};

// Function to display the main menu
void mainmenu() {
    cout << "\n==================== Buenos Nachos Menu ====================\n";
    cout << "| Item                        | Price      |\n";
    cout << "-------------------------------------------------------------\n";
    cout << "| 1. Delectable Nachos        | RM 25.90   |\n";
    cout << "| 2. Mouthwatering Tacos      | RM 19.90   |\n";
    cout << "| 3. Yummy Enchiladas         | RM 18.90   |\n";
    cout << "| 4. Divine Quesadillas       | RM 23.90   |\n";
    cout << "| 5. Exquisite Elote          | RM 14.90   |\n";
    cout << "| 6. Superb Horchata          | RM  9.90   |\n";
    cout << "| 7. Supreme Agua de Jamaica  | RM  8.90   |\n";
    cout << "| 8. Epic Michelada           | RM 12.90   |\n";
    cout << "| 9. Awesome Champurrado      | RM 11.50   |\n";
    cout << "=============================================================\n";
}

// Function to validate if the input is a positive integer
bool ValidNum(string str) {
    for (char c : str) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

// Promo code validation (no matter upper/lowercase/mix)
bool checkPromo(string promo) {
    string validCode = "nachos4life"; //valid model (all lowercase)
    if (promo.length() != validCode.length()) {
        return false;
    }
    for (int i = 0; i < promo.length(); ++i) {
        char p = promo[i];
        if (p >= 'A' && p <= 'Z') {
            p = p + ('a' - 'A'); // convert to lowercase
        }
        if (p != validCode[i]) {
            return false;
        }
    }
    return true;
}

// Convert char in string to integer
int strToint(string str) {
    int result = 0;
    for (char c : str) {
        result = result * 10 + (c - '0'); // Convert each character to an integer
    }
    return result;
}

// Convert a string to a double
double strTodouble(string str) {
    double result = 0.0;
    bool decimalPointFound = false;
    double decimalPlaceValue = 1.0;

    for (char c : str) {
        if (c >= '0' && c <= '9') {
            if (!decimalPointFound) {
                result = result * 10 + (c - '0');
            } else {
                decimalPlaceValue *= 0.1;
                result += (c - '0') * decimalPlaceValue;
            }
        } else if (c == '.') {
            if (decimalPointFound) {
                return -1.0;
            }
            decimalPointFound = true;
        } else {
            return -1.0;
        }
    }
    return result;
}

// Calculate and dispense the change
void changeDispense(double total, double payment) {
    double change = payment - total;
    cout << "Change to dispense: RM " << fixed << setprecision(2) << change << "\n";

    double faceval[10] = {100.0, 50.0, 20.0, 10.0, 5.0, 1.0, 0.50, 0.20, 0.10, 0.05};
    int counts[10] = {0};

    for (int i = 0; i < 10; i++) {
        while (change >= faceval[i]) {
            change -= faceval[i];
            change = round(change * 100) / 100.0;
            counts[i]++;
        }
    }

    cout << "Dispensed Change:\n";
    for (int i = 0; i < 10; i++) {
        if (counts[i] > 0) {
            cout << counts[i] << " x RM " << fixed << setprecision(2) << faceval[i] << "\n";
        }
    }
}

// Function to display bill before payment
void Bill(const string& name, const string& phoneNum, int quantities[], double prices[], int itemCount, double total) {
    cout << "\n========================= Order Bill ========================\n";
    cout << "Name: " << name << "\n";
    cout << "Phone: " << phoneNum << "\n";
    cout << "============================= Items ===========================\n";
    for (int i = 0; i < itemCount; ++i) {
        if (quantities[i] > 0) {
            cout << "Item " << (i + 1) << ": " << quantities[i] << " x RM " << fixed << setprecision(2) << prices[i]
                 << " = RM " << fixed << setprecision(2) << prices[i] * quantities[i] << "\n";
        }
    }
    cout << "============================= Total ==========================\n";
    cout << "Total: RM " << fixed << setprecision(2) << total << "\n";
    cout << "==============================================================\n";
}

int main() {
    string name, phoneNum;
    const int itemCount = 9;
    double prices[itemCount] = {25.90, 19.90, 18.90, 23.90, 14.90, 9.90, 8.90, 12.90, 11.50};

    vector<Order> previousOrders;

    cout << "============================================================\n";
    cout << "                   Welcome to Buenos Nachos!               \n";
    cout << "============================================================\n";
    cout << "Please enter your name: ";
    getline(cin, name);

    while (true) {
        cout << "Please enter your phone number (digits only): ";
        getline(cin, phoneNum);
        if (ValidNum(phoneNum)) break;
        cout << "Invalid phone number. Please try again.\n";
    }

    cout << "\nThank you, " << name << "! Your phone number has been recorded.\n";

    char addMore;
    do {
        mainmenu();
        double total = 0.0;
        int quantities[itemCount] = {0};

        while (true) {
            string orderstr;
            cout << "Enter the item code (1 to " << itemCount << ", or 0 to finish): ";
            cin >> orderstr;

            if (!ValidNum(orderstr)) {
                cout << "Invalid input! Please enter a valid number.\n";
                continue;
            }

            int order = strToint(orderstr);
            if (order == 0) break;
            if (order < 1 || order > itemCount) {
                cout << "Invalid item code. Try again.\n";
                continue;
            }

            string quantitystr;
            cout << "Enter the quantity (press 0 to cancel): ";
            cin >> quantitystr;

            if (!ValidNum(quantitystr)) {
                cout << "Invalid input! Please enter a valid number.\n";
                continue;
            }

            int quantity = strToint(quantitystr);
            if (quantity <= 0) continue;

            int index = order - 1;
            quantities[index] += quantity;
            total += prices[index] * quantity;
        }

        Bill(name, phoneNum, quantities, prices, itemCount, total);

        Order currentOrder = {0};
        for (int i = 0; i < itemCount; i++) {
            currentOrder.quantities[i] = quantities[i];
        }
        currentOrder.total = total;
        previousOrders.push_back(currentOrder);

        string promo;
        cout << "Do you have a promo code? (yes/no): ";
        cin >> promo;

        if (promo == "yes" || promo == "Yes") {
            bool promoApplied = false;
            for (int attempt = 0; attempt < 2; ++attempt) {
                cout << "Enter your promo code: ";
                cin >> promo;

                if (checkPromo(promo)) {
                    total *= 0.92;
                    total = round(total * 20) / 20;
                    cout << "Promo applied! New total = RM " << fixed << setprecision(2) << total << "\n";
                    promoApplied = true;
                    break;
                } else {
                    cout << "Invalid promo code. ";
                    if (attempt == 0) {
                        cout << "You have one more attempt left.\n";
                    } else {
                        cout << "Promo code validation failed.\n";
                    }
                }
            }
        }

        double payment;
        string paymentstr;
        while (true) {
            cout << "Enter payment amount: RM ";
            cin >> paymentstr;

            payment = strTodouble(paymentstr);
            if (payment < 0) {
                cout << "Invalid payment amount. It must be a positive number.\n";
                continue;
            } else if (payment < total) {
                cout << "Insufficient payment. Try again.\n";
                continue;
            }
            break;
        }

        changeDispense(total, payment);

        cout << "\nWould you like to place another order? (y/n): ";
        cin >> addMore;

    } while (addMore == 'y' || addMore == 'Y');

    cout << "\nThank you for your purchases, " << name << "!\n";
    cout << "Have a great day!\n";

    // Optional: Print out all previous orders at the end
    cout << "\nPrevious Orders:\n";
    for (size_t i = 0; i < previousOrders.size(); ++i) {
        cout << "Order " << (i + 1) << ": Total = RM " << fixed << setprecision(2) << previousOrders[i].total << "\n";
        for (int j = 0; j < itemCount; ++j) {
            if (previousOrders[i].quantities[j] > 0) {
                cout << "Item " << (j + 1) << ": " << previousOrders[i].quantities[j] << "\n";
            }
        }
        cout << "--------------------------\n";
    }

    return 0;
}
