#include "Book.hpp"
#include <iostream>
#include <iomanip>  // dodajemy do używania std::setw()

Book::Book(std::string t, std::string a) {
    title = t;
    author = a;
}

void Book::displayInfo() const {
    // Ustawiamy stałą szerokość dla każdej kolumny
    const int columnWidth = 20;

    // Wyświetlamy tabelkę
    std::cout << "+---------------------+------------------------+" << std::endl;
    std::cout << "| " << std::setw(columnWidth) << std::left << "Tytul" << " | " 
              << std::setw(columnWidth) << std::left << title << " |" << std::endl;
    std::cout << "+---------------------+------------------------+" << std::endl;
    std::cout << "| " << std::setw(columnWidth) << std::left << "Autor" << " | " 
              << std::setw(columnWidth) << std::left << author << " |" << std::endl;
    std::cout << "+---------------------+------------------------+" << std::endl;
}
