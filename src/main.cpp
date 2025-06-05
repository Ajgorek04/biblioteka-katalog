#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

using namespace std;

const int MAX_BOOKS = 100;
const int MAX_SUBCATEGORIES = 20;
const int MAX_CATEGORIES = 50;

struct Book {
    string title;
    string author;
    string isbn;
};

struct Category {
    string name;
    Book books[MAX_BOOKS];
    int bookCount;

    Category* subcategories[MAX_SUBCATEGORIES];
    int subcategoryCount;

    Category() {
        bookCount = 0;
        subcategoryCount = 0;
        for (int i = 0; i < MAX_SUBCATEGORIES; ++i) {
            subcategories[i] = nullptr;
        }
    }
};

Category* categories[MAX_CATEGORIES];
int categoryCount = 0;

void saveCategory(ofstream &file, const Category* cat, int depth = 0) {
    string indent(depth * 2, ' ');
    file << indent << "[CATEGORY] " << cat->name << '\n';
    for (int i = 0; i < cat->bookCount; ++i) {
        const Book& book = cat->books[i];
        file << indent << "  [BOOK] " << book.title << " | " << book.author << " | " << book.isbn << '\n';
    }
    for (int i = 0; i < cat->subcategoryCount; ++i) {
        saveCategory(file, cat->subcategories[i], depth + 1);
    }
}

void saveToFile() {
    ofstream file("katalog.txt");
    for (int i = 0; i < categoryCount; ++i) {
        saveCategory(file, categories[i]);
    }
    file.close();
}

bool parseBook(const string &line, Book &book) {
    size_t start = line.find("[BOOK] ");
    if (start == string::npos) return false;
    string content = line.substr(start + 7);
    stringstream ss(content);
    getline(ss, book.title, '|');
    getline(ss, book.author, '|');
    getline(ss, book.isbn);

    auto trim = [](string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch){ return !isspace(ch); }));
        s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !isspace(ch); }).base(), s.end());
    };
    trim(book.title);
    trim(book.author);
    trim(book.isbn);
    return true;
}

void loadFromFile() {
    ifstream file("katalog.txt");
    if (!file) return;

    string line;
    Category* stack[100];
    int stackDepth = 0;

    for (int i = 0; i < categoryCount; ++i) {
        function<void(Category*)> freeCategory = [&](Category* cat) {
            for (int j = 0; j < cat->subcategoryCount; ++j) {
                freeCategory(cat->subcategories[j]);
            }
            delete cat;
        };
        freeCategory(categories[i]);
        categories[i] = nullptr;
    }
    categoryCount = 0;

    while (getline(file, line)) {
        size_t depth = 0;
        while (depth < line.size() && line[depth] == ' ') depth++;
        string trimmed = line.substr(depth);

        if (trimmed.rfind("[CATEGORY] ", 0) == 0) {
            string catName = trimmed.substr(11);
            Category* cat = new Category();
            cat->name = catName;

            int level = depth / 2;
            if (level == 0) {
                categories[categoryCount++] = cat;
                stackDepth = 1;
                stack[0] = cat;
            } else {
                while (stackDepth > level) stackDepth--;
                Category* parent = stack[stackDepth - 1];
                if (parent->subcategoryCount < MAX_SUBCATEGORIES) {
                    parent->subcategories[parent->subcategoryCount++] = cat;
                    stack[stackDepth++] = cat;
                } else {
                    cout << "Za duzo podkategorii w: " << parent->name << endl;
                    delete cat;
                }
            }
        } else if (trimmed.rfind("[BOOK] ", 0) == 0) {
            Book book;
            if (parseBook(trimmed, book)) {
                Category* currentCat = stack[stackDepth - 1];
                if (currentCat->bookCount < MAX_BOOKS) {
                    currentCat->books[currentCat->bookCount++] = book;
                } else {
                    cout << "Za duzo ksiazek w kategorii: " << currentCat->name << endl;
                }
            }
        }
    }
    file.close();
}

void printCategory(const Category* cat, int depth = 0) {
    string indent(depth * 2, ' ');
    cout << indent << "- " << cat->name << endl;
    for (int i = 0; i < cat->bookCount; ++i) {
        const Book &book = cat->books[i];
        cout << indent << "  * " << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")" << endl;
    }
    for (int i = 0; i < cat->subcategoryCount; ++i) {
        printCategory(cat->subcategories[i], depth + 1);
    }
}

void addBookToCategory(Category* cat) {
    if (cat->bookCount >= MAX_BOOKS) {
        cout << "Nie mozna dodac wiecej ksiazek do tej kategorii.\n";
        return;
    }
    Book book;
    cout << "Tytul: "; getline(cin, book.title);
    cout << "Autor: "; getline(cin, book.author);
    cout << "ISBN: "; getline(cin, book.isbn);
    cat->books[cat->bookCount++] = book;
}

void addSubcategory(Category* cat) {
    if (cat->subcategoryCount >= MAX_SUBCATEGORIES) {
        cout << "Nie mozna dodac wiecej podkategorii.\n";
        return;
    }
    Category* sub = new Category();
    cout << "Nazwa podkategorii: "; getline(cin, sub->name);
    cat->subcategories[cat->subcategoryCount++] = sub;
}

void editCategory(Category* cat) {
    cout << "Aktualna nazwa: " << cat->name << endl;
    cout << "Nowa nazwa (ENTER aby nie zmieniac): ";
    string newName;
    getline(cin, newName);
    if (!newName.empty()) {
        cat->name = newName;
    }
}

void editBook(Book &book) {
    cout << "Edytujesz ksiazke: " << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")\n";
    cout << "Nowy tytul (ENTER aby nie zmieniac): ";
    string t;
    getline(cin, t);
    if (!t.empty()) book.title = t;

    cout << "Nowy autor (ENTER aby nie zmieniac): ";
    getline(cin, t);
    if (!t.empty()) book.author = t;

    cout << "Nowy ISBN (ENTER aby nie zmieniac): ";
    getline(cin, t);
    if (!t.empty()) book.isbn = t;
}

bool removeCategory(Category** cats, int &count, Category* target) {
    for (int i = 0; i < count; ++i) {
        if (cats[i] == target) {
            // Rekurencyjne usuwanie podkategorii i zwolnienie pamieci
            function<void(Category*)> freeCategory = [&](Category* cat) {
                for (int j = 0; j < cat->subcategoryCount; ++j) {
                    freeCategory(cat->subcategories[j]);
                }
                delete cat;
            };
            freeCategory(cats[i]);

            for (int j = i; j < count - 1; ++j) {
                cats[j] = cats[j + 1];
            }
            count--;
            return true;
        } else if (removeCategory(cats[i]->subcategories, cats[i]->subcategoryCount, target)) {
            return true;
        }
    }
    return false;
}

void removeBookFromCategory(Category* cat, int index) {
    for (int i = index; i < cat->bookCount - 1; ++i) {
        cat->books[i] = cat->books[i + 1];
    }
    cat->bookCount--;
}

struct NavigationStackItem {
    Category* cat;
    Category* parent;
};

Category* navigateCategory(Category* root = nullptr) {
    // Drzewo nawigacji: w kazdym kroku wiemy parenta (nullptr dla korzenia)
    NavigationStackItem stack[100];
    int depth = 0;

    if (root == nullptr) {
        // Startujemy od glownej listy kategorii
        stack[0] = { nullptr, nullptr }; // brak wybranej kategorii, parent null
    } else {
        stack[0] = { root, nullptr };
    }
    depth = 1;

    while (depth > 0) {
        Category* current = stack[depth - 1].cat;
        Category* parent = stack[depth - 2 >= 0 ? depth - 2 : 0].cat;

        cout << "\nAktualna kategoria: " << (current ? current->name : "Glowna lista kategorii") << "\n";

        Category** list;
        int count;
        if (current == nullptr) {
            list = categories;
            count = categoryCount;
        } else {
            list = current->subcategories;
            count = current->subcategoryCount;
        }

        if (count == 0) {
            cout << "Brak podkategorii.\n";
        } else {
            cout << "Podkategorie:\n";
            for (int i = 0; i < count; ++i) {
                cout << " " << (i + 1) << ". " << list[i]->name << "\n";
            }
        }

        cout << "Opcje:\n";
        cout << " 0. Dodaj podkategorie tutaj\n";
        cout << "-1. Wstecz\n";
        cout << " e. Edytuj kategorie\n";
        cout << " u. Usun kategorie\n";
        cout << "Wybierz numer podkategorii lub opcje: ";

        string input;
        getline(cin, input);

        if (input == "0") {
            if (current == nullptr) {
                // Dodaj kategorie do glownej listy
                if (categoryCount >= MAX_CATEGORIES) {
                    cout << "Nie mozna dodac wiecej kategorii.\n";
                    continue;
                }
                Category* cat = new Category();
                cout << "Nazwa nowej kategorii: ";
                getline(cin, cat->name);
                categories[categoryCount++] = cat;
            } else {
                addSubcategory(current);
            }
        }
        else if (input == "-1") {
            if (depth == 1) {
                // Na najwyzszym poziomie wychodzimy
                return nullptr;
            }
            depth--;
        }
        else if (input == "e" || input == "E") {
            if (count == 0) {
                cout << "Brak podkategorii do edycji.\n";
                continue;
            }
            cout << "Wybierz numer podkategorii do edycji: ";
            string idxStr;
            getline(cin, idxStr);
            int idx = stoi(idxStr);
            if (idx < 1 || idx > count) {
                cout << "Nieprawidlowy wybor.\n";
                continue;
            }
            editCategory(list[idx - 1]);
        }
        else if (input == "u" || input == "U") {
            if (count == 0) {
                cout << "Brak podkategorii do usuniecia.\n";
                continue;
            }
            cout << "Wybierz numer podkategorii do usuniecia: ";
            string idxStr;
            getline(cin, idxStr);
            int idx = stoi(idxStr);
            if (idx < 1 || idx > count) {
                cout << "Nieprawidlowy wybor.\n";
                continue;
            }
            if (removeCategory(list, count, list[idx - 1])) {
                cout << "Kategoria usunieta.\n";
            } else {
                cout << "Nie udalo sie usunac kategorii.\n";
            }
        }
        else {
            // Sprawdzmy czy to numer podkategorii
            int num = 0;
            try {
                num = stoi(input);
            } catch (...) {
                cout << "Nieznana opcja.\n";
                continue;
            }
            if (num < 1 || num > count) {
                cout << "Nieprawidlowy numer.\n";
                continue;
            }
            // Wchodzimy do wybranej podkategorii
            stack[depth++] = { list[num - 1], current };
        }
    }
    return nullptr; // nigdy tu nie dotrzemy
}

void bookMenu(Category* cat) {
    if (cat == nullptr) return;

    while (true) {
        cout << "\nKategoria: " << cat->name << "\n";
        if (cat->bookCount == 0) {
            cout << "Brak ksiazek w tej kategorii.\n";
        } else {
            cout << "Ksiazki:\n";
            for (int i = 0; i < cat->bookCount; ++i) {
                cout << " " << (i + 1) << ". " << cat->books[i].title << " by " << cat->books[i].author << " (ISBN: " << cat->books[i].isbn << ")\n";
            }
        }

        cout << "Opcje:\n";
        cout << " 0. Dodaj ksiazke\n";
        cout << "-1. Powrot\n";
        cout << " e. Edytuj ksiazke\n";
        cout << " u. Usun ksiazke\n";
        cout << "Wybierz opcje lub numer ksiazki: ";

        string input;
        getline(cin, input);

        if (input == "0") {
            addBookToCategory(cat);
        }
        else if (input == "-1") {
            return;
        }
        else if (input == "e" || input == "E") {
            if (cat->bookCount == 0) {
                cout << "Brak ksiazek do edycji.\n";
                continue;
            }
            cout << "Wybierz numer ksiazki do edycji: ";
            string idxStr;
            getline(cin, idxStr);
            int idx = stoi(idxStr);
            if (idx < 1 || idx > cat->bookCount) {
                cout << "Nieprawidlowy wybor.\n";
                continue;
            }
            editBook(cat->books[idx - 1]);
        }
        else if (input == "u" || input == "U") {
            if (cat->bookCount == 0) {
                cout << "Brak ksiazek do usuniecia.\n";
                continue;
            }
            cout << "Wybierz numer ksiazki do usuniecia: ";
            string idxStr;
            getline(cin, idxStr);
            int idx = stoi(idxStr);
            if (idx < 1 || idx > cat->bookCount) {
                cout << "Nieprawidlowy wybor.\n";
                continue;
            }
            removeBookFromCategory(cat, idx - 1);
            cout << "Ksiazka usunieta.\n";
        }
        else {
            // Sprawdzamy czy numer ksiazki
            int num = 0;
            try {
                num = stoi(input);
            } catch (...) {
                cout << "Nieznana opcja.\n";
                continue;
            }
            if (num < 1 || num > cat->bookCount) {
                cout << "Nieprawidlowy numer.\n";
                continue;
            }
            // Pokaz szczegoly ksiazki
            Book& b = cat->books[num - 1];
            cout << "Tytul: " << b.title << "\nAutor: " << b.author << "\nISBN: " << b.isbn << "\n";
        }
    }
}

void mainMenu() {
    while (true) {
        cout << "\n--- MENU GLOWNE ---\n";
        cout << "1. Przegladaj kategorie\n";
        cout << "2. Dodaj kategorie\n";
        cout << "3. Wyswietl wszystkie kategorie i ksiazki\n";
        cout << "4. Zapisz do pliku\n";
        cout << "5. Wczytaj z pliku\n";
        cout << "0. Wyjdz\n";
        cout << "Wybierz opcje: ";

        string input;
        getline(cin, input);

        if (input == "1") {
            Category* cat = navigateCategory(nullptr);
            if (cat != nullptr) {
                bookMenu(cat);
            }
        }
        else if (input == "2") {
            if (categoryCount >= MAX_CATEGORIES) {
                cout << "Nie mozna dodac wiecej kategorii.\n";
                continue;
            }
            Category* cat = new Category();
            cout << "Nazwa nowej kategorii: ";
            getline(cin, cat->name);
            categories[categoryCount++] = cat;
        }
        else if (input == "3") {
            if (categoryCount == 0) {
                cout << "Brak kategorii.\n";
            }
            for (int i = 0; i < categoryCount; ++i) {
                printCategory(categories[i]);
            }
        }
        else if (input == "4") {
            saveToFile();
            cout << "Zapisano do pliku.\n";
        }
        else if (input == "5") {
            loadFromFile();
            cout << "Wczytano z pliku.\n";
        }
        else if (input == "0") {
            cout << "Koniec programu.\n";
            break;
        }
        else {
            cout << "Nieznana opcja.\n";
        }
    }
}

int main() {
    loadFromFile();
    mainMenu();

    // Zwolnienie pamieci przed wyjsciem
    for (int i = 0; i < categoryCount; ++i) {
        function<void(Category*)> freeCategory = [&](Category* cat) {
            for (int j = 0; j < cat->subcategoryCount; ++j) {
                freeCategory(cat->subcategories[j]);
            }
            delete cat;
        };
        freeCategory(categories[i]);
    }

    return 0;
}
