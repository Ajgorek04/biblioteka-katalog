#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Book {
    string title;
    string author;
    int year;
    Book* next;
};

struct Subcategory {
    string name;
    Book* booksHead;
    Subcategory* next;
};

struct Category {
    string name;
    Subcategory* subcategoriesHead;
    Category* next;
};

int countCategories(Category* head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}

Category* getCategoryByIndex(Category* head, int index) {
    int i = 1;
    while (head && i < index) {
        head = head->next;
        i++;
    }
    return head;
}

void addCategory(Category*& head) {
    string name;
    cout << "Podaj nazwę nowej kategorii: ";
    getline(cin >> ws, name);

    Category* newCat = new Category;
    newCat->name = name;
    newCat->subcategoriesHead = nullptr;
    newCat->next = head;
    head = newCat;

    ofstream file("kategorie.txt", ios::app);
    if (file.is_open()) {
        file << name << "\n";
        file.close();
        cout << "Dodano i zapisano kategorię: " << name << "\n";
    } else {
        cout << "Błąd zapisu do pliku!\n";
    }
}

void showCategoriesWithSubcategories(Category* head) {
    if (!head) {
        cout << "Brak kategorii do wyświetlenia.\n";
        return;
    }

    int i = 1;
    while (head) {
        cout << i++ << ". " << head->name << "\n";
        Subcategory* sub = head->subcategoriesHead;
        if (!sub) {
            cout << "   (brak podkategorii)\n";
        } else {
            while (sub) {
                cout << "   - " << sub->name << "\n";
                sub = sub->next;
            }
        }
        head = head->next;
    }
}


void loadCategories(Category*& head) {
    ifstream file("kategorie.txt");
    if (!file.is_open()) {
        cout << "Brak pliku z kategoriami. Zostanie utworzony przy zapisie.\n";
        return;
    }

    string name;
    while (getline(file, name)) {
        if (!name.empty()) {
            Category* newCat = new Category;
            newCat->name = name;
            newCat->subcategoriesHead = nullptr;
            newCat->next = head;
            head = newCat;
        }
    }

    file.close();
}

void saveAllCategories(Category* head) {
    ofstream file("kategorie.txt");
    if (file.is_open()) {
        while (head != nullptr) {
            file << head->name << "\n";
            head = head->next;
        }
        file.close();
    } else {
        cout << "Błąd podczas zapisu do pliku.\n";
    }
}

void listCategories(Category* head) {
    if (head == nullptr) {
        cout << "Brak kategorii.\n";
        return;
    }

    cout << "\nLista kategorii:\n\n";
    int i = 1;
    while (head != nullptr) {
        cout << i++ << ". " << head->name << "\n";
        head = head->next;
    }
}

void addBookToSubcategory(Category* head) {
    if (!head) {
        cout << "Brak kategorii.\n";
        return;
    }

    // Wybierz kategorię
    listCategories(head);
    cout << "Wybierz kategorię (numer): ";
    int catIndex;
    cin >> catIndex;

    Category* selectedCategory = getCategoryByIndex(head, catIndex);
    if (!selectedCategory) {
        cout << "Nieprawidłowa kategoria.\n";
        return;
    }

    // Wybierz podkategorię
    Subcategory* sub = selectedCategory->subcategoriesHead;
    if (!sub) {
        cout << "Ta kategoria nie ma żadnych podkategorii.\n";
        return;
    }

    cout << "\nPodkategorie w kategorii \"" << selectedCategory->name << "\":\n";
    int i = 1;
    Subcategory* subIter = sub;
    while (subIter) {
        cout << i++ << ". " << subIter->name << "\n";
        subIter = subIter->next;
    }

    cout << "Wybierz podkategorię (numer): ";
    int subIndex;
    cin >> subIndex;

    subIter = sub;
    for (int j = 1; j < subIndex && subIter; j++) {
        subIter = subIter->next;
    }

    if (!subIter) {
        cout << "Nieprawidłowa podkategoria.\n";
        return;
    }

    // Wprowadź dane książki
    string title, author;
    int year;

    cout << "Tytuł: ";
    getline(cin >> ws, title);
    cout << "Autor: ";
    getline(cin >> ws, author);
    cout << "Rok wydania: ";
    cin >> year;

    // Dodaj do listy książek
    Book* newBook = new Book{title, author, year, nullptr};
    newBook->next = subIter->booksHead;
    subIter->booksHead = newBook;

    cout << "Dodano książkę do podkategorii \"" << subIter->name << "\".\n";
}

void deleteCategory(Category*& head) {
    int total = countCategories(head);
    if (total == 0) {
        cout << "Brak kategorii do usunięcia.\n";
        return;
    }

    listCategories(head);
    cout << "Podaj numer kategorii do usunięcia (1-" << total << "): ";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > total) {
        cout << "Nieprawidłowy numer!\n";
        return;
    }

    Category* current = head;
    Category* prev = nullptr;

    for (int i = 1; i < choice; i++) {
        prev = current;
        current = current->next;
    }

    if (prev == nullptr) {
        head = current->next;
    } else {
        prev->next = current->next;
    }

    delete current;
    saveAllCategories(head);

    cout << "Kategoria usunięta.\n";
}

void addSubcategory(Category* head) {
    int total = countCategories(head);
    if (total == 0) {
        cout << "Brak kategorii. Najpierw dodaj kategorię.\n";
        return;
    }

    listCategories(head);
    cout << "Wybierz kategorię, do której chcesz dodać podkategorię (1-" << total << "): ";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > total) {
        cout << "Nieprawidłowy numer!\n";
        return;
    }

    Category* selected = getCategoryByIndex(head, choice);
    string subName;
    cout << "Podaj nazwę podkategorii: ";
    getline(cin >> ws, subName);

    Subcategory* newSub = new Subcategory;
    newSub->name = subName;
    newSub->booksHead = nullptr;
    newSub->next = selected->subcategoriesHead;
    selected->subcategoriesHead = newSub;

    cout << "Dodano podkategorię '" << subName << "' do kategorii '" << selected->name << "'.\n";
}


void showMenu() {
    cout << "\n--- MENU ---\n";
    cout << "1. Dodaj kategorię\n";
    cout << "2. Wyświetl kategorie i podkategorie\n";
    cout << "3. Usuń kategorię\n";
    cout << "4. Dodaj podkategorię\n";
    cout << "5. Dodaj książkę do podkategorii\n";
    cout << "0. Wyjście\n";
    cout << "Wybierz opcję: ";
}

int main() {
    Category* categoryList = nullptr;

    loadCategories(categoryList);

    int choice;
    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                addCategory(categoryList);
                break;
            case 2:
                showCategoriesWithSubcategories(categoryList);
                break;
            case 3:
                deleteCategory(categoryList);
                break;
            case 4:
                addSubcategory(categoryList);
                break;
            case 5:
                addBookToSubcategory(categoryList);
                break;
            case 0:
                cout << "Zamykanie programu...\n";
                break;
            default:
                cout << "Nieznana opcja.\n";
        }

    } while (choice != 0);

    return 0;
}
