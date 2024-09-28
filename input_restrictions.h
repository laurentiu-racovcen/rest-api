#ifndef _INPUT_RESTRICTIONS_
#define _INPUT_RESTRICTIONS_

#include <stdlib.h>
#include <string>

#define LOWEST_SUITABLE_CHAR 33
#define HIGHEST_SUITABLE_CHAR 126
#define LOWEST_DIGIT_CHAR 48
#define HIGHEST_DIGIT_CHAR 57
#define HIGHEST_LOWERCASE_LETTER_CHAR 122
#define LOWEST_UPPERCASE_LETTER_CHAR 65
#define HIGHEST_UPPERCASE_LETTER_CHAR 90
#define LOWEST_LOWERCASE_LETTER_CHAR 97
#define SPACE_CHAR 32
#define POINT_CHAR 46

// Function that checks if an username is valid for registering an account
bool username_is_valid(string username) {
    // check if the username is empty
    if (username.empty()) {
        return false;
    }
    // check if the username contains unsuitable characters;
    // the suitable ASCII characters are in the range 33-126
    for (size_t i = 0; i < username.length(); i++) {
        if (username[i] < LOWEST_SUITABLE_CHAR || username[i] > HIGHEST_SUITABLE_CHAR) {
            return false;
        }
    }

    return true;
}

// Function that checks if a password is valid for registering an account
bool password_is_valid(string password) {
    // check if the password is empty
    if (password.empty()) {
        return false;
    }
    // check if the password contains unsuitable characters;
    // the suitable ASCII characters are in the range 33-126
    for (size_t i = 0; i < password.length(); i++) {
        if (password[i] < LOWEST_SUITABLE_CHAR || password[i] > HIGHEST_SUITABLE_CHAR) {
            return false;
        }
    }

    return true;
}

// Function that checks if a book id is valid
bool book_id_is_valid(string id) {
    // check if the id is empty
    if (id.empty()) {
        return false;
    }
    // check if the id contains unsuitable characters;
    // the suitable ASCII digit characters are in the range 48-57
    for (size_t i = 0; i < id.length(); i++) {
        if (id[i] < LOWEST_DIGIT_CHAR || id[i] > HIGHEST_DIGIT_CHAR) {
            return false;
        }
    }
    return true;
}

// Function that checks if a book title is valid
bool book_title_is_valid(string title) {
    // check if the title is empty
    if (title.empty()) {
        return false;
    }

    if (title[0] == ' ') {
        // the title starts with a space
        return false;
    }

    if (title[title.length()-1] == ' ') {
        // the title ends with a space
        return false;
    }

    // check if the title contains unsuitable characters;
    // the suitable ASCII characters are in the ranges:
    // 33-122 (suitable characters), 32 (space)
    for (size_t i = 0; i < title.length(); i++) {
        if (!((title[i] >= LOWEST_SUITABLE_CHAR && title[i] <= HIGHEST_LOWERCASE_LETTER_CHAR)
                || (title[i] == SPACE_CHAR))
            )
        {
            return false;
        }
    }
    return true;
}

// Function that checks if an author name is valid
bool book_author_is_valid(string author) {
    if (author[0] == ' ') {
        // the author name starts with a space
        return false;
    }

    if (author[author.length()-1] == ' ') {
        // the author name ends with a space
        return false;
    }

    // check if the author name contains unsuitable characters;
    // the suitable ASCII characters are in the ranges:
    // 65-90 (uppercase letters), 97-122 (lowercase letters), 32 (space), 46 (point)
    for (size_t i = 0; i < author.length(); i++) {
        if (!((author[i] >= LOWEST_UPPERCASE_LETTER_CHAR && author[i] <= HIGHEST_UPPERCASE_LETTER_CHAR)
            || (author[i] >= LOWEST_LOWERCASE_LETTER_CHAR && author[i] <= HIGHEST_LOWERCASE_LETTER_CHAR)
            || (author[i] == SPACE_CHAR)
            || (author[i] == POINT_CHAR))
            )
        {
            return false;
        }
    }
    return true;
}

// Function that checks if a book genre is valid
bool book_genre_is_valid(string genre) {
    // check if the genre contains unsuitable characters;
    // the suitable ASCII characters are in the ranges:
    // 65-90 (uppercase letters), 97-122 (lowercase letters), 32 (space)
    for (size_t i = 0; i < genre.length(); i++) {
        if (!((genre[i] >= LOWEST_LOWERCASE_LETTER_CHAR && genre[i] <= HIGHEST_LOWERCASE_LETTER_CHAR)
            || (genre[i] >= LOWEST_UPPERCASE_LETTER_CHAR && genre[i] <= HIGHEST_UPPERCASE_LETTER_CHAR)
            || (genre[i] == SPACE_CHAR))
            )
        {
            return false;
        }
    }
    return true;
}

// Function that checks if a book publisher is valid
bool book_publisher_is_valid(string publisher) {
    // check if the author name contains unsuitable characters;
    // the suitable ASCII characters are in the ranges:
    // 65-90 (uppercase letters), 97-122 (lowercase letters), 32 (space), 46 (point)
    for (size_t i = 0; i < publisher.length(); i++) {
        if (!((publisher[i] >= LOWEST_UPPERCASE_LETTER_CHAR && publisher[i] <= HIGHEST_UPPERCASE_LETTER_CHAR)
            || (publisher[i] >= LOWEST_LOWERCASE_LETTER_CHAR && publisher[i] <= HIGHEST_LOWERCASE_LETTER_CHAR)
            || (publisher[i] == SPACE_CHAR)
            || (publisher[i] == POINT_CHAR))
            )
        {
            return false;
        }
    }
    return true;
}

// Function that checks if a book page count is valid
bool book_page_count_is_valid(string page_count) {
    // check if the page count is empty
    if (page_count.empty()) {
        return false;
    }

    // check if the page_count contains unsuitable characters;
    // the suitable ASCII characters are in the ranges:
    // 48-57 (digits)
    for (size_t i = 0; i < page_count.length(); i++) {
        if (page_count[i] < LOWEST_DIGIT_CHAR || page_count[i] > HIGHEST_DIGIT_CHAR) {
            return false;
        }
    }
    return true;
}

#endif
