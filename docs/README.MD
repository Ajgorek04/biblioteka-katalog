# 📚 Katalog Tematyczny Biblioteki

Projekt katalogu tematycznego biblioteki stworzony w języku **C++**. Aplikacja umożliwia zarządzanie książkami w bibliotece, klasyfikowanie ich w kategorie oraz podkategorie, dodawanie tytułów i autorów oraz umożliwia przeglądanie i edytowanie tych książek.

## 📑 Spis Treści

- [Opis Projektu](#opis-projektu)
- [Technologie](#technologie)
- [Instalacja](#instalacja)
- [Użycie](#użycie)
- [Struktura Plików](#struktura-plików)
- [Licencja](#licencja)

---

## 💡 Opis Projektu

Aplikacja pozwala na tworzenie i przeglądanie katalogu książek, które są przypisane do różnych kategorii tematycznych. Użytkownicy mogą dodawać książki, przypisywać je do głównych kategorii, a także do podkategorii, zarządzać danymi książek oraz wyszukiwać je według różnych kryteriów.

---

## ⚙️ Technologie

Projekt został stworzony w **C++**, używając standardu **C++17**.

- **C++** - język programowania
- **Standard C++17** - nowoczesne funkcje C++

---

## 📥 Instalacja

### Wymagania:

- **Kompilator C++** (np. **g++**).
- **Git** do sklonowania repozytorium.

### 🛠 Kroki Instalacyjne:

1.  **Zainstaluj g++ (kompilator C++):**

    **Na Windowsie:**

    - Zainstaluj **MinGW** (zawiera g++) lub użyj **MSYS2**.

    **Na Linuxie i macOS:**

    - **Dla Ubuntu**:
      ```bash
      sudo apt install g++
      ```
    - **Dla macOS**:
      ```bash
      brew install gcc
      ```

2.  **Sklonuj repozytorium:**
    Jeśli masz zainstalowanego **Git**, sklonuj repozytorium na swoim komputerze:

    ```bash
    git clone https://github.com/twoje_imie/biblioteka-katalog.git
    ```

3.  **Wejdź do katalogu z projektem:**
    Po sklonowaniu repozytorium, przejdź do katalogu projektu:

    ```bash
    cd biblioteka-katalog
    ```

4.  **Zbuduj projekt:**
    W katalogu `src`, uruchom poniższe polecenie, aby skompilować plik wykonywalny:

    **Na Windowsie:**

    ```bash
    g++ src/*.cpp -Iinclude -o katalog
    ./katalog
    ```

    **Na Linuxie/macOS:**

    ```bash
    g++ src/*.cpp -Iinclude -o katalog
    ./katalog
    ```

5.  **Uruchom program:**  
    Po skompilowaniu programu, uruchom go za pomocą odpowiedniego polecenia:

    **Na Windowsie:**

    ```bash
    .\katalog.exe
    ```

    **Na Linuxie/macOS:**

    ```bash
    ./katalog
    ```

---

## ⚙️ Użycie

Po uruchomieniu programu użytkownik może:

- Dodać książki do katalogu.
- Przypisać książki do głównych kategorii (np. Literatura, Historia, Nauka).
- Przypisać książki do podkategorii.
- Przeglądać książki w wybranej kategorii lub podkategorii.
- Edytować dane książek (np. zmiana tytułu, autora, roku wydania).

---
