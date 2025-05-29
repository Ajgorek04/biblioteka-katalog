#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
using namespace std;

struct Book {
    string title;
    string author;
    int year;
};

struct Subcategory {
    string name;
    vector<Subcategory*> subcategories;
    vector<Book> books;
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

int getNumberInput(const string& prompt) {
    int num;
    while (true) {
        cout << prompt;
        if (cin >> num) return num;
        cout << "Nieprawidlowy numer. Sprobuj ponownie.\n";
        clearInput();
    }
}

void addCategory() {
    cout << "Podaj nazwe nowej kategorii: ";
    string name;
    clearInput();
    getline(cin, name);
    categories.push_back({name});
    cout << "Dodano kategorie: " << name << "\n";
}

void displaySubcategories(const vector<Subcategory*>& subs, int depth = 1) {
    for (auto* sub : subs) {
        cout << string(depth * 2, ' ') << "- " << sub->name << "\n";
        for (const auto& book : sub->books) {
            cout << string((depth + 1) * 2, ' ') << "* " << book.title << " - " << book.author << " (" << book.year << ")\n";
        }
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
        int idx = getNumberInput("Wybierz podkategorie (0 - cofnij): ");
        if (idx == 0) return nullptr;
        if (idx > 0 && idx <= (int)subs.size()) {
            Subcategory* chosen = subs[idx - 1];
            if (chosen->subcategories.empty()) return chosen;
            int opt = getNumberInput("1. Dodaj tutaj\n2. Wejdz dalej\nWybierz: ");
            if (opt == 1) return chosen;
            else if (opt == 2) subs = chosen->subcategories;
        } else {
            cout << "Nieprawidlowy wybor.\n";
        }
    }
}

void addSubcategory(vector<Subcategory*>& subs) {
    cout << "Podaj nazwe podkategorii: ";
    clearInput();
    string name;
    getline(cin, name);
    Subcategory* sub = new Subcategory{name};
    subs.push_back(sub);
    cout << "Dodano podkategorie: " << name << "\n";
}

void addSubcategoryToCategory() {
    if (categories.empty()) {
        cout << "Brak kategorii.\n";
        return;
    }
    cout << "Dostepne kategorie:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << i + 1 << ". " << categories[i].name << "\n";
    }
    int index = getNumberInput("Wybierz kategorie (numer): ");
    if (index < 1 || index > (int)categories.size()) {
        cout << "Nieprawidlowy wybor.\n";
        return;
    }
    Category& selected = categories[index - 1];

    while (true) {
        cout << "\nGdzie chcesz dodac podkategorie?\n";
        cout << "1. Do kategorii \"" << selected.name << "\"\n";
        cout << "2. Do istniejacej podkategorii\n";
        cout << "0. Anuluj\n";
        int option = getNumberInput("Wybierz opcje: ");
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
            cout << "Nieprawidlowa opcja.\n";
        }
    }
}

void addBookToSubcategory() {
    if (categories.empty()) {
        cout << "Brak kategorii.\n";
        return;
    }
    cout << "Dostepne kategorie:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << i + 1 << ". " << categories[i].name << "\n";
    }
    int index = getNumberInput("Wybierz kategorie (numer): ");
    if (index < 1 || index > (int)categories.size()) {
        cout << "Nieprawidlowy wybor.\n";
        return;
    }
    Category& selected = categories[index - 1];
    if (selected.subcategories.empty()) {
        cout << "Brak podkategorii.\n";
        return;
    }
    Subcategory* sub = chooseSubcategory(selected.subcategories);
    if (!sub) return;

    clearInput();
    Book book;
    cout << "Podaj tytul ksiazki: ";
    getline(cin, book.title);
    cout << "Podaj autora: ";
    getline(cin, book.author);
    book.year = getNumberInput("Podaj rok wydania: ");

    sub->books.push_back(book);
    cout << "Dodano ksiazke: " << book.title << " - " << book.author << " (" << book.year << ")\n";
}

void saveSub(ofstream& out, vector<Subcategory*> subs, int depth) {
    for (auto* sub : subs) {
        out << string(depth, '-') << "SUB:" << sub->name << "\n";
        for (const auto& book : sub->books) {
            out << string(depth + 1, '-') << "BOOK:" << book.title << "|" << book.author << "|" << book.year << "\n";
        }
        saveSub(out, sub->subcategories, depth + 1);
    }
}

void saveToFile(const string& filename) {
    ofstream out(filename);
    if (!out) {
        cout << "Blad zapisu do pliku.\n";
        return;
    }

    for (const auto& cat : categories) {
        out << "CAT:" << cat.name << "\n";
        saveSub(out, cat.subcategories, 1);
    }

    out.close();
}

void loadFromFile(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cout << "Brak pliku do wczytania. Tworzymy pusty katalog.\n";
        return;
    }

    categories.clear();
    string line;
    vector<Subcategory*> stack;

    while (getline(in, line)) {
        size_t pos = line.find(':');
        if (pos == string::npos) continue;

        string type = line.substr(pos - 3, 3);
        string content = line.substr(pos + 1);
        int depth = 0;
        while (depth < (int)line.size() && line[depth] == '-') ++depth;

        if (line.substr(0, 4) == "CAT:") {
            categories.push_back({content});
            stack.clear();
        } else if (type == "SUB") {
            Subcategory* sub = new Subcategory{content};
            if (depth == 1) {
                categories.back().subcategories.push_back(sub);
            } else if (depth > 1 && (int)stack.size() >= depth - 1) {
                stack[depth - 2]->subcategories.push_back(sub);
            }
            if ((int)stack.size() >= depth) stack.resize(depth);
            if ((int)stack.size() < depth) stack.push_back(sub);
            else stack[depth - 1] = sub;
        } else if (type == "OOK") {
            istringstream ss(content);
            string title, author, yearStr;
            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, yearStr);
            Book book{title, author, stoi(yearStr)};
            if (!stack.empty() && (int)stack.size() >= depth - 1) {
                stack[depth - 2]->books.push_back(book);
            }
        }
    }

    in.close();
}

void editMenu() {
    cout << "=== EDYCJA ===\n";
    cout << "1. Edytuj kategorie\n";
    cout << "2. Edytuj podkategorie\n";
    cout << "3. Edytuj ksiazke\n";
    int opt = getNumberInput("Wybierz: ");
    clearInput();

    if (opt == 1) {
        if (categories.empty()) {
            cout << "Brak kategorii do edycji.\n";
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].name << "\n";
        int idx = getNumberInput("Wybierz kategorie: ");
        clearInput();
        if (idx >= 1 && idx <= (int)categories.size()) {
            cout << "Nowa nazwa: ";
            string newName;
            getline(cin, newName);
            categories[idx - 1].name = newName;
            cout << "Zmieniono nazwe kategorii.\n";
        }
    } else if (opt == 2) {
        if (categories.empty()) {
            cout << "Brak kategorii.\n";
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].name << "\n";
        int catIdx = getNumberInput("Wybierz kategorie: ");
        clearInput();
        if (catIdx < 1 || catIdx > (int)categories.size()) return;

        Subcategory* sub = chooseSubcategory(categories[catIdx - 1].subcategories);
        if (!sub) {
            cout << "Nie wybrano podkategorii.\n";
            return;
        }
        cout << "Nowa nazwa podkategorii: ";
        string newName;
        getline(cin, newName);
        sub->name = newName;
        cout << "Zmieniono nazwe podkategorii.\n";
    } else if (opt == 3) {
        if (categories.empty()) {
            cout << "Brak kategorii.\n";
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].name << "\n";
        int catIdx = getNumberInput("Wybierz kategorie: ");
        clearInput();
        if (catIdx < 1 || catIdx > (int)categories.size()) return;

        Subcategory* sub = chooseSubcategory(categories[catIdx - 1].subcategories);
        if (!sub || sub->books.empty()) {
            cout << "Brak ksiazek w wybranej podkategorii.\n";
            return;
        }

        for (size_t i = 0; i < sub->books.size(); ++i)
            cout << i + 1 << ". " << sub->books[i].title << "\n";
        int bIdx = getNumberInput("Wybierz ksiazke: ");
        clearInput();
        if (bIdx < 1 || bIdx > (int)sub->books.size()) return;

        Book& b = sub->books[bIdx - 1];
        cout << "Nowy tytul: "; getline(cin, b.title);
        cout << "Nowy autor: "; getline(cin, b.author);
        b.year = getNumberInput("Nowy rok: ");
        cout << "Zmieniono dane ksiazki.\n";
    } else {
        cout << "Nieznana opcja.\n";
    }
}

void deleteMenu() {
    cout << "=== USUWANIE ===\n";
    cout << "1. Usun kategorie\n";
    cout << "2. Usun podkategorie\n";
    cout << "3. Usun ksiazke\n";
    int opt = getNumberInput("Wybierz: ");
    clearInput();

    if (opt == 1) {
        if (categories.empty()) {
            cout << "Brak kategorii do usuniecia.\n";
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].name << "\n";
        int idx = getNumberInput("Wybierz do usuniecia: ");
        clearInput();
        if (idx >= 1 && idx <= (int)categories.size()) {
            // Zwolnij pamięć podkategorii
            auto deleteSubcategories = [](vector<Subcategory*>& subs) {
                for (auto* sub : subs) {
                    deleteSubcategories(sub->subcategories);
                    delete sub;
                }
                subs.clear();
            };
            deleteSubcategories(categories[idx - 1].subcategories);

            categories.erase(categories.begin() + (idx - 1));
            cout << "Usunieto kategorie.\n";
        }
    } else if (opt == 2) {
        if (categories.empty()) {
            cout << "Brak kategorii.\n";
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].name << "\n";
        int catIdx = getNumberInput("Wybierz kategorie: ");
        clearInput();
        if (catIdx < 1 || catIdx > (int)categories.size()) return;

        vector<Subcategory*>* current = &categories[catIdx - 1].subcategories;
        vector<Subcategory*>* parent = nullptr;

        while (true) {
            if (current->empty()) {
                cout << "Brak podkategorii do usuniecia.\n";
                return;
            }
            for (size_t i = 0; i < current->size(); ++i)
                cout << i + 1 << ". " << (*current)[i]->name << "\n";
            cout << "0. Cofnij\n";
            int sIdx = getNumberInput("Wybierz podkategorie do usuniecia (0 - cofnij): ");
            clearInput();
            if (sIdx == 0) return;
            if (sIdx >= 1 && sIdx <= (int)current->size()) {
                cout << "1. Usun\n2. Wejdz glebiej\nWybierz: ";
                int act;
                cin >> act;
                clearInput();
                if (act == 1) {
                    // usuń rekurencyjnie podkategorie
                    auto deleteSubcategories = [](vector<Subcategory*>& subs) {
                        for (auto* sub : subs) {
                            deleteSubcategories(sub->subcategories);
                            delete sub;
                        }
                        subs.clear();
                    };
                    deleteSubcategories((*current)[sIdx - 1]->subcategories);
                    delete (*current)[sIdx - 1];
                    current->erase(current->begin() + (sIdx - 1));
                    cout << "Usunieto podkategorie.\n";
                    return;
                } else if (act == 2) {
                    parent = current;
                    current = &(*current)[sIdx - 1]->subcategories;
                }
            } else {
                cout << "Nieprawidlowy wybor.\n";
            }
        }
    } else if (opt == 3) {
        if (categories.empty()) {
            cout << "Brak kategorii.\n";
            return;
        }
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].name << "\n";
        int catIdx = getNumberInput("Wybierz kategorie: ");
        clearInput();
        if (catIdx < 1 || catIdx > (int)categories.size()) return;

        Subcategory* sub = chooseSubcategory(categories[catIdx - 1].subcategories);
        if (!sub || sub->books.empty()) {
            cout << "Brak ksiazek w wybranej podkategorii.\n";
            return;
        }

        for (size_t i = 0; i < sub->books.size(); ++i)
            cout << i + 1 << ". " << sub->books[i].title << "\n";
        int bIdx = getNumberInput("Wybierz ksiazke do usuniecia: ");
        clearInput();
        if (bIdx >= 1 && bIdx <= (int)sub->books.size()) {
            sub->books.erase(sub->books.begin() + (bIdx - 1));
            cout << "Usunieto ksiazke.\n";
        }
    } else {
        cout << "Nieznana opcja.\n";
    }
}

int main() {
    loadFromFile("dane.txt");

    while (true) {
        cout << "\n--- MENU ---\n";
        cout << "1. Dodaj kategorie\n";
        cout << "2. Dodaj podkategorie\n";
        cout << "3. Dodaj ksiazke\n";
        cout << "4. Wyswietl katalog\n";
        cout << "5. Edytuj kategorie/podkategorie/ksiazki\n";
        cout << "6. Usun kategorie/podkategorie/ksiazki\n";
        cout << "7. Zapisz do pliku\n";
        cout << "0. Wyjscie\n";

        int choice = getNumberInput("Wybierz opcje: ");
        clearInput();

        switch (choice) {
            case 1: addCategory(); break;
            case 2: addSubcategoryToCategory(); break;
            case 3: addBookToSubcategory(); break;
            case 4: displayAll(); break;
            case 5: editMenu(); break;
            case 6: deleteMenu(); break;
            case 7: saveToFile("dane.txt"); cout << "Zapisano do pliku.\n"; break;
            case 0:
                cout << "Koniec programu.\n";
                // Zwolnienie pamięci
                for (auto& cat : categories) {
                    function<void(vector<Subcategory*>&)> delSubcats = [&](vector<Subcategory*>& subs) {
                        for (auto* s : subs) {
                            delSubcats(s->subcategories);
                            delete s;
                        }
                        subs.clear();
                    };
                    delSubcats(cat.subcategories);
                }
                categories.clear();
                return 0;
            default:
                cout << "Nieznana opcja.\n";
        }
    }

    return 0;
}
