#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <functional>
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
            int year = stoi(yearStr);
            if (!stack.empty()) {
                stack.back()->books.push_back({title, author, year});
            }
        }
    }
}

void deleteMenu() {
    if (categories.empty()) {
        cout << "Brak kategorii do usuniecia.\n";
        return;
    }

    cout << "Kategorie:\n";
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << i + 1 << ". " << categories[i].name << "\n";
    }

    int index = getNumberInput("Wybierz kategorie do usuniecia (0 - anuluj): ");
    if (index == 0) return;
    if (index < 1 || index > (int)categories.size()) {
        cout << "Nieprawidlowy wybor.\n";
        return;
    }

    // Poprawiona deklaracja funkcji rekurencyjnej usuwania podkategorii:
    function<void(vector<Subcategory*>&)> deleteSubcategories = [&](vector<Subcategory*>& subs) {
        for (auto* sub : subs) {
            deleteSubcategories(sub->subcategories);
            delete sub;
        }
        subs.clear();
    };

    deleteSubcategories(categories[index - 1].subcategories);
    categories.erase(categories.begin() + index - 1);
    cout << "Usunieto kategorie.\n";
}

int main() {
    loadFromFile("katalog.txt");

    while (true) {
        cout << "\n--- MENU ---\n";
        cout << "1. Dodaj kategorie\n";
        cout << "2. Dodaj podkategorie\n";
        cout << "3. Dodaj ksiazke\n";
        cout << "4. Wyswietl wszystkie kategorie\n";
        cout << "5. Usun kategorie\n";
        cout << "0. Wyjscie\n";

        int choice = getNumberInput("Wybierz opcje: ");

        switch (choice) {
            case 1:
                addCategory();
                break;
            case 2:
                addSubcategoryToCategory();
                break;
            case 3:
                addBookToSubcategory();
                break;
            case 4:
                displayAll();
                break;
            case 5:
                deleteMenu();
                break;
            case 0:
                {
                    // Funkcja usuwająca wszystkie podkategorie przed wyjściem
                    function<void(vector<Subcategory*>&)> delSubcats = [&](vector<Subcategory*>& subs) {
                        for (auto* s : subs) {
                            delSubcats(s->subcategories);
                            delete s;
                        }
                        subs.clear();
                    };
                    for (auto& cat : categories) {
                        delSubcats(cat.subcategories);
                    }
                    categories.clear();
                    saveToFile("katalog.txt");
                    cout << "Zapisano i zakonczono program.\n";
                }
                return 0;
            default:
                cout << "Nieprawidlowa opcja.\n";
                break;
        }
    }
}
