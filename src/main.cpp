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

void menu(vector<Category> &categories) {
    while (true) {
        cout << "\nMENU:\n";
        cout << "1. Wyswietl katalog\n";
        cout << "2. Dodaj kategorie\n";
        cout << "3. Dodaj podkategorie\n";
        cout << "4. Dodaj ksiazke\n";
        cout << "5. Zapisz i wyjdz\n";
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
            if (cat) {
                addSubcategory(*cat);
            }
        } else if (choice == "4") {
            Category* cat = selectCategoryFromTree(categories);
            if (cat) {
                addBookToCategory(*cat);
            }
        } else if (choice == "5") {
            saveToFile(categories);
            cout << "Zapisano katalog do pliku. Do zobaczenia!\n";
            break;
        } else {
            cout << "Nieznana opcja.\n";
        }
    }
}

int main() {
    vector<Category> categories;
    loadFromFile(categories);
    menu(categories);
    return 0;
}
