#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>

class Book {
private:
    std::string title;
    std::string author;

public:
    // Konstruktor
    Book(std::string t, std::string a);

    void displayInfo() const;
};

#endif