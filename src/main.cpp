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
    // trim spaces from left and right of title, author, isbn
    auto trim = [](string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch){ return !isspace(ch); }));
        s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !isspace(ch); }).base(), s.end());
    };
    trim(book.title);
    trim(book.author);
    trim(book.isbn);
    return true;
}

// Funkcja do wczytywania z pliku - tworzymy hierarchię kategorii dynamicznie
void loadFromFile() {
    ifstream file("katalog.txt");
    if (!file) return;  // plik nie istnieje, startujemy z pustą listą

    string line;
    Category* stack[100];
    int stackDepth = 0;

    // Najpierw usuwamy stare kategorie, jeśli jakieś były
    for (int i = 0; i < categoryCount; ++i) {
        // zwalniamy pamięć dynamicznie alokowanych kategorii (wczytanych poniżej)
        // zrobimy prostą rekurencję na zwalnianie pamięci:
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

            int level = depth / 2;  // 2 spacje na poziom
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

// 🔽 NOWA FUNKCJA do wybierania dowolnej kategorii/podkategorii z drzewa (używa tablicy wskaźników)
struct FlatCategory {
    string path;
    Category* cat;
};

FlatCategory flatList[1000];
int flatListCount = 0;

void flattenCategories(Category* cat, const string &path) {
    string currentPath = path.empty() ? cat->name : path + " > " + cat->name;
    flatList[flatListCount++] = {currentPath, cat};
    for (int i = 0; i < cat->subcategoryCount; ++i) {
        flattenCategories(cat->subcategories[i], currentPath);
    }
}

Category* selectCategoryFromTree() {
    flatListCount = 0;
    for (int i = 0; i < categoryCount; ++i) {
        flattenCategories(categories[i], "");
    }

    if (flatListCount == 0) {
        cout << "Brak kategorii.\n";
        return nullptr;
    }

    cout << "Dostepne kategorie:\n";
    for (int i = 0; i < flatListCount; ++i) {
        cout << i + 1 << ". " << flatList[i].path << '\n';
    }

    cout << "Wybierz numer kategorii: ";
    string input;
    getline(cin, input);
    int index = stoi(input);

    if (index < 1 || index > flatListCount) {
        cout << "Nieprawidlowy wybor.\n";
        return nullptr;
    }

    return flatList[index - 1].cat;
}

void editCategory(Category* cat) {
    cout << "Aktualna nazwa: " << cat->name << endl;
    cout << "Nowa nazwa: ";
    string newName;
    getline(cin, newName);
    if (!newName.empty()) {
        cat->name = newName;
    }
}

void editBook(Book &book) {
    cout << "Edytujesz ksiazke: " << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")\n";
    cout << "Nowy tytul (lub ENTER, aby nie zmieniac): ";
    string t;
    getline(cin, t);
    if (!t.empty()) book.title = t;

    cout << "Nowy autor (lub ENTER): ";
    getline(cin, t);
    if (!t.empty()) book.author = t;

    cout << "Nowy ISBN (lub ENTER): ";
    getline(cin, t);
    if (!t.empty()) book.isbn = t;
}

void editMenu() {
    Category* cat = selectCategoryFromTree();
    if (!cat) return;

    cout << "1. Edytuj nazwe kategorii\n";
    cout << "2. Edytuj ksiazke\n";
    cout << "Wybor: ";
    string choice;
    getline(cin, choice);

    if (choice == "1") {
        editCategory(cat);
    } else if (choice == "2") {
        if (cat->bookCount == 0) {
            cout << "Brak ksiazek w tej kategorii.\n";
            return;
        }

        for (int i = 0; i < cat->bookCount; ++i) {
            cout << i + 1 << ". " << cat->books[i].title << " by " << cat->books[i].author << " (ISBN: " << cat->books[i].isbn << ")\n";
        }

        cout << "Wybierz numer ksiazki do edycji: ";
        string input;
        getline(cin, input);
        int idx = stoi(input);

        if (idx < 1 || idx > cat->bookCount) {
            cout << "Nieprawidlowy wybor.\n";
            return;
        }

        editBook(cat->books[idx - 1]);
    }
}

bool removeCategory(Category** cats, int &count, Category* target) {
    for (int i = 0; i < count; ++i) {
        if (cats[i] == target) {
            // Usuwamy rekurencyjnie podkategorie
            function<void(Category*)> freeCategory = [&](Category* cat) {
                for (int j = 0; j < cat->subcategoryCount; ++j) {
                    freeCategory(cat->subcategories[j]);
                }
                delete cat;
            };
            freeCategory(cats[i]);

            // przesuwamy elementy tablicy w lewo
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

void deleteMenu() {
    cout << "1. Usun cala kategorie/podkategorie\n";
    cout << "2. Usun ksiazke z kategorii\n";
    cout << "Wybor: ";
    string choice;
    getline(cin, choice);

    if (choice == "1") {
        Category* target = selectCategoryFromTree();
        if (!target) return;

        if (removeCategory(categories, categoryCount, target)) {
            cout << "Kategoria usunieta.\n";
        } else {
            cout << "Nie udalo sie usunac.\n";
        }

    } else if (choice == "2") {
        Category* cat = selectCategoryFromTree();
        if (!cat || cat->bookCount == 0) {
            cout << "Brak ksiazek.\n";
            return;
        }

        for (int i = 0; i < cat->bookCount; ++i) {
            cout << i + 1 << ". " << cat->books[i].title << " by " << cat->books[i].author << " (ISBN: " << cat->books[i].isbn << ")\n";
        }

        cout << "Wybierz numer ksiazki do usuniecia: ";
        string input;
        getline(cin, input);
        int idx = stoi(input);

        if (idx < 1 || idx > cat->bookCount) {
            cout << "Nieprawidlowy wybor.\n";
            return;
        }

        for (int i = idx - 1; i < cat->bookCount - 1; ++i) {
            cat->books[i] = cat->books[i + 1];
        }
        cat->bookCount--;
        cout << "Ksiazka usunieta.\n";
    }
}

void menu() {
    while (true) {
        cout << "\nMENU:\n";
        cout << "1. Wyswietl katalog\n";
        cout << "2. Dodaj kategorie\n";
        cout << "3. Dodaj podkategorie\n";
        cout << "4. Dodaj ksiazke\n";
        cout << "5. Zapisz i wyjdz\n";
        cout << "6. Edytuj katalog\n";
        cout << "7. Usun z katalogu\n";
        cout << "Wybor: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            for (int i = 0; i < categoryCount; ++i) {
                printCategory(categories[i]);
            }
        } else if (choice == "2") {
            if (categoryCount >= MAX_CATEGORIES) {
                cout << "Nie mozna dodac wiecej kategorii.\n";
                continue;
            }
            Category* cat = new Category();
            cout << "Nazwa kategorii: ";
            getline(cin, cat->name);
            categories[categoryCount++] = cat;
        } else if (choice == "3") {
            Category* cat = selectCategoryFromTree();
            if (cat) addSubcategory(cat);
        } else if (choice == "4") {
            Category* cat = selectCategoryFromTree();
            if (cat) addBookToCategory(cat);
        } else if (choice == "5") {
            saveToFile();
            cout << "Zapisano katalog. Do zobaczenia!\n";
            break;
        } else if (choice == "6") {
            editMenu();
        } else if (choice == "7") {
            deleteMenu();
        } else {
            cout << "Nieznana opcja.\n";
        }
    }
}

int main() {
    loadFromFile();
    menu();

    // zwalniamy pamięć przy wyjściu
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
