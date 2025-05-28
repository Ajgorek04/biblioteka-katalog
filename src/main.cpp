#include <iostream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

struct Subcategory {
    string name;
    vector<Subcategory*> subcategories;
    vector<string> books;
};

struct Category {
    string name;
    vector<Subcategory*> subcategories;
};

vector<Category> categories;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void addCategory() {
    cout << "Podaj nazwę nowej kategorii: ";
    string name;
    clearInput();
    getline(cin, name);
    categories.push_back({name});
    cout << "Dodano kategorię: " << name << "\n";
}

void displaySubcategories(const vector<Subcategory*>& subs, int depth = 1) {
    for (auto* sub : subs) {
        cout << string(depth * 2, ' ') << "- " << sub->name << "\n";
        displaySubcategories(sub->subcategories, depth + 1);
    }
}

void displayAll() {
    int i = 1;
    for (auto& cat : categories) {
        cout << i++ << ". " << cat.name << "\n";
        if (cat.subcategories.empty()) {
            cout << "   (Brak podkategorii)\n";
        } else {
            displaySubcategories(cat.subcategories);
        }
    }
}

Subcategory* chooseSubcategory(vector<Subcategory*>& subs) {
    while (true) {
        for (size_t i = 0; i < subs.size(); ++i) {
            cout << i + 1 << ". " << subs[i]->name << "\n";
        }
        cout << "Wybierz podkategorię (0 - cofnij): ";
        int idx;
        cin >> idx;
        if (idx == 0) return nullptr;
        if (idx > 0 && idx <= subs.size()) {
            Subcategory* chosen = subs[idx - 1];
            if (chosen->subcategories.empty()) return chosen;
            cout << "1. Dodaj tutaj\n2. Wejdź dalej\nWybierz: ";
            int opt;
            cin >> opt;
            if (opt == 1) return chosen;
            else if (opt == 2) subs = chosen->subcategories;
        } else {
            cout << "Nieprawidłowy wybór.\n";
        }
    }
}

void addSubcategory(vector<Subcategory*>& subs) {
    cout << "Podaj nazwę podkategorii: ";
    clearInput();
    string name;
    getline(cin, name);
    Subcategory* sub = new Subcategory{name};
    subs.push_back(sub);
    cout << "Dodano podkategorię: " << name << "\n";
}

void addSubcategoryToCategory() {
    if (categories.empty()) {
        cout << "Brak kategorii.\n";
        return;
    }
    cout << "Dostępne kategorie:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << i + 1 << ". " << categories[i].name << "\n";
    }
    cout << "Wybierz kategorię (numer): ";
    int index;
    cin >> index;
    if (index < 1 || index > categories.size()) {
        cout << "Nieprawidłowy wybór.\n";
        return;
    }
    Category& selected = categories[index - 1];

    while (true) {
        cout << "\nGdzie chcesz dodać podkategorię?\n";
        cout << "1. Do kategorii \"" << selected.name << "\"\n";
        cout << "2. Do istniejącej podkategorii\n";
        cout << "0. Anuluj\n";
        cout << "Wybierz opcję: ";
        int option;
        cin >> option;
        if (option == 0) return;
        else if (option == 1) {
            addSubcategory(selected.subcategories);
            return;
        } else if (option == 2) {
            if (selected.subcategories.empty()) {
                cout << "Brak podkategorii.\n";
            } else {
                Subcategory* sub = chooseSubcategory(selected.subcategories);
                if (sub) {
                    addSubcategory(sub->subcategories);
                    return;
                }
            }
        } else {
            cout << "Nieprawidłowa opcja.\n";
        }
    }
}

void addBookToSubcategory() {
    if (categories.empty()) {
        cout << "Brak kategorii.\n";
        return;
    }
    cout << "Dostępne kategorie:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << i + 1 << ". " << categories[i].name << "\n";
    }
    cout << "Wybierz kategorię (numer): ";
    int index;
    cin >> index;
    if (index < 1 || index > categories.size()) {
        cout << "Nieprawidłowy wybór.\n";
        return;
    }
    Category& selected = categories[index - 1];
    if (selected.subcategories.empty()) {
        cout << "Brak podkategorii.\n";
        return;
    }
    Subcategory* sub = chooseSubcategory(selected.subcategories);
    if (!sub) return;

    cout << "Podaj tytuł książki: ";
    clearInput();
    string title;
    getline(cin, title);
    sub->books.push_back(title);
    cout << "Dodano książkę: " << title << "\n";
}

int main() {
    while (true) {
        cout << "\n--- MENU ---\n";
        cout << "1. Dodaj kategorię\n";
        cout << "2. Dodaj podkategorię (rekurencyjnie)\n";
        cout << "3. Dodaj książkę do podkategorii\n";
        cout << "4. Wyświetl wszystko\n";
        cout << "0. Wyjście\n";
        cout << "Wybierz opcję: ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1: addCategory(); break;
            case 2: addSubcategoryToCategory(); break;
            case 3: addBookToSubcategory(); break;
            case 4: displayAll(); break;
            case 0: return 0;
            default: cout << "Nieprawidłowa opcja.\n"; break;
        }
    }
}
