#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
using namespace std;

struct Book {
    string title;
    string author;
    string isbn;
};

struct Category {
    string name;
    vector<Book> books;
    vector<Category> subcategories;
};

void saveCategory(ofstream &file, const Category &cat, int depth = 0) {
    string indent(depth * 2, ' ');
    file << indent << "[CATEGORY] " << cat.name << '\n';
    for (const auto &book : cat.books) {
        file << indent << "  [BOOK] " << book.title << " | " << book.author << " | " << book.isbn << '\n';
    }
    for (const auto &sub : cat.subcategories) {
        saveCategory(file, sub, depth + 1);
    }
}

void saveToFile(const vector<Category> &categories) {
    ofstream file("katalog.txt");
    for (const auto &cat : categories) {
        saveCategory(file, cat);
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
    book.title = string(book.title.begin(), remove(book.title.begin(), book.title.end(), ' '));
    book.author = string(book.author.begin(), remove(book.author.begin(), book.author.end(), ' '));
    book.isbn = string(book.isbn.begin(), remove(book.isbn.begin(), book.isbn.end(), ' '));
    return true;
}

void loadFromFile(vector<Category> &categories) {
    ifstream file("katalog.txt");
    string line;
    vector<Category*> stack;
    while (getline(file, line)) {
        size_t depth = 0;
        while (line[depth] == ' ') depth++;
        line = line.substr(depth);
        if (line.rfind("[CATEGORY] ", 0) == 0) {
            Category cat;
            cat.name = line.substr(11);
            if (depth == 0) {
                categories.push_back(cat);
                stack = { &categories.back() };
            } else {
                while (stack.size() > depth / 2) stack.pop_back();
                stack.back()->subcategories.push_back(cat);
                stack.push_back(&stack.back()->subcategories.back());
            }
        } else if (line.rfind("[BOOK] ", 0) == 0) {
            Book book;
            if (parseBook(line, book)) {
                stack.back()->books.push_back(book);
            }
        }
    }
    file.close();
}

void printCategory(const Category &cat, int depth = 0) {
    string indent(depth * 2, ' ');
    cout << indent << "- " << cat.name << endl;
    for (const auto &book : cat.books) {
        cout << indent << "  * " << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")" << endl;
    }
    for (const auto &sub : cat.subcategories) {
        printCategory(sub, depth + 1);
    }
}

void addBookToCategory(Category &cat) {
    Book book;
    cout << "Tytul: "; getline(cin, book.title);
    cout << "Autor: "; getline(cin, book.author);
    cout << "ISBN: "; getline(cin, book.isbn);
    cat.books.push_back(book);
}

void addSubcategory(Category &cat) {
    Category sub;
    cout << "Nazwa podkategorii: "; getline(cin, sub.name);
    cat.subcategories.push_back(sub);
}

// 🔽 NOWA FUNKCJA do wybierania dowolnej kategorii/podkategorii z drzewa
Category* selectCategoryFromTree(vector<Category>& categories) {
    vector<pair<string, Category*>> flatList;

    function<void(Category&, string)> flatten = [&](Category& cat, string path) {
        string currentPath = path.empty() ? cat.name : path + " > " + cat.name;
        flatList.emplace_back(currentPath, &cat);
        for (auto &sub : cat.subcategories) {
            flatten(sub, currentPath);
        }
    };

    for (auto &cat : categories) {
        flatten(cat, "");
    }

    if (flatList.empty()) {
        cout << "Brak kategorii.\n";
        return nullptr;
    }

    cout << "Dostepne kategorie:\n";
    for (size_t i = 0; i < flatList.size(); ++i) {
        cout << i + 1 << ". " << flatList[i].first << '\n';
    }

    cout << "Wybierz numer kategorii: ";
    string input;
    getline(cin, input);
    int index = stoi(input);

    if (index < 1 || index > (int)flatList.size()) {
        cout << "Nieprawidlowy wybor.\n";
        return nullptr;
    }

    return flatList[index - 1].second;
}

void editMenu(vector<Category> &categories);
void deleteMenu(vector<Category> &categories);

void menu(vector<Category> &categories) {
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
            for (const auto &cat : categories) printCategory(cat);
        } else if (choice == "2") {
            Category cat;
            cout << "Nazwa kategorii: "; getline(cin, cat.name);
            categories.push_back(cat);
        } else if (choice == "3") {
            Category* cat = selectCategoryFromTree(categories);
            if (cat) addSubcategory(*cat);
        } else if (choice == "4") {
            Category* cat = selectCategoryFromTree(categories);
            if (cat) addBookToCategory(*cat);
        } else if (choice == "5") {
            saveToFile(categories);
            cout << "Zapisano katalog. Do zobaczenia!\n";
            break;
        } else if (choice == "6") {
            editMenu(categories);
        } else if (choice == "7") {
            deleteMenu(categories);
        } else {
            cout << "Nieznana opcja.\n";
        }
    }
}

void editCategory(Category &cat) {
    cout << "Aktualna nazwa: " << cat.name << endl;
    cout << "Nowa nazwa: ";
    string newName;
    getline(cin, newName);
    if (!newName.empty()) {
        cat.name = newName;
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

void editMenu(vector<Category> &categories) {
    Category* cat = selectCategoryFromTree(categories);
    if (!cat) return;

    cout << "1. Edytuj nazwe kategorii\n";
    cout << "2. Edytuj ksiazke\n";
    cout << "Wybor: ";
    string choice;
    getline(cin, choice);

    if (choice == "1") {
        editCategory(*cat);
    } else if (choice == "2") {
        if (cat->books.empty()) {
            cout << "Brak ksiazek w tej kategorii.\n";
            return;
        }

        for (size_t i = 0; i < cat->books.size(); ++i) {
            cout << i + 1 << ". " << cat->books[i].title << " by " << cat->books[i].author << " (ISBN: " << cat->books[i].isbn << ")\n";
        }

        cout << "Wybierz numer ksiazki do edycji: ";
        string input;
        getline(cin, input);
        int idx = stoi(input);

        if (idx < 1 || idx > (int)cat->books.size()) {
            cout << "Nieprawidlowy wybor.\n";
            return;
        }

        editBook(cat->books[idx - 1]);
    }
}

void deleteMenu(vector<Category> &categories) {
    cout << "1. Usun cala kategorie/podkategorie\n";
    cout << "2. Usun ksiazke z kategorii\n";
    cout << "Wybor: ";
    string choice;
    getline(cin, choice);

    if (choice == "1") {
        Category* target = selectCategoryFromTree(categories);
        if (!target) return;

        function<bool(vector<Category>&)> removeCategory = [&](vector<Category>& cats) -> bool {
            for (auto it = cats.begin(); it != cats.end(); ++it) {
                if (&(*it) == target) {
                    cats.erase(it);
                    return true;
                } else if (removeCategory(it->subcategories)) {
                    return true;
                }
            }
            return false;
        };

        if (removeCategory(categories)) {
            cout << "Kategoria usunieta.\n";
        } else {
            cout << "Nie udalo sie usunac.\n";
        }

    } else if (choice == "2") {
        Category* cat = selectCategoryFromTree(categories);
        if (!cat || cat->books.empty()) {
            cout << "Brak ksiazek.\n";
            return;
        }

        for (size_t i = 0; i < cat->books.size(); ++i) {
            cout << i + 1 << ". " << cat->books[i].title << " by " << cat->books[i].author << " (ISBN: " << cat->books[i].isbn << ")\n";
        }

        cout << "Wybierz numer ksiazki do usuniecia: ";
        string input;
        getline(cin, input);
        int idx = stoi(input);

        if (idx < 1 || idx > (int)cat->books.size()) {
            cout << "Nieprawidlowy wybor.\n";
            return;
        }

        cat->books.erase(cat->books.begin() + idx - 1);
        cout << "Ksiazka usunieta.\n";
    }
}


int main() {
    vector<Category> categories;
    loadFromFile(categories);
    menu(categories);
    return 0;
}
