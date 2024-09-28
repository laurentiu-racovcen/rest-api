#ifndef _UTILS_
#define _UTILS_

#include <stdlib.h>
#include <string>

// Function that returns the value of the first cookie from the "Cookie" header
string get_cookie(char *response) {
    string searched_string = "Set-Cookie: ";

    // to process lines
    istringstream stream(response);
    string current_line;

    // where the cookies will be stored
    string cookies;

    while (getline(stream, current_line)) {
        if (current_line.find(searched_string) != string::npos) {
            cookies = current_line;
        }
    }

    if (!cookies.empty()) {
        // extract session id cookie (first cookie from the list)
        // from all set cookies

        unsigned first = cookies.find('=') + 1; // + 1 for not including '='
        unsigned last = cookies.find(';');

        string cookie = cookies.substr(first, last-first);

        return cookie;
    }

    // "Set-Cookie: " substring has not been found; return empty string
    return "";
}

// Function that returns the response code of a HTTP response string
int get_response_code(char* response) {
    char* code_start = response + 9;
    char code[3] = {};
    strncpy(code, code_start, 3);
    return atoi(code);
}

#endif
