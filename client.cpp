#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "json.hpp"
#include "helpers.h"
#include "requests.h"
#include "input_restrictions.h"
#include "utils.h"

using namespace std;

// Function that handles all types of errors
// and prints the parsed error from the response
void handle_response_errors(char *response) {
    char* extracted_json = basic_extract_json_response(response);

    if (extracted_json != NULL) {
        json json_response = json::parse(extracted_json);
        string json_string;

        json_response["error"].get_to(json_string);
        cout << "\033[31m" << "ERROR: " << json_string << "\n" << "\033[0m";
    } else {
        cout << "\033[31m" << "ERROR: Nu a fost returnat un obiect de tip json!" << "\n" << "\033[0m";
    }
}

// Function that registers an user
void handle_register(string ip, unsigned short port, string cookie) {
    string username, password;

    bool username_is_ok = true;
    cout << "\033[34m" << "username=" << "\033[0m";
    getline(cin, username);
    if (!username_is_valid(username)) {
        username_is_ok = false;
    }

    cout << "\033[34m" << "password=" << "\033[0m";
    getline(cin, password);
    if (!password_is_valid(password) || !username_is_ok) {
        cout << "\033[31m" << "ERROR: username sau/si password au un format incorect!\n" << "\033[0m";
        return;
    }

    if (!cookie.empty()) {
        cout << "\033[31m" << "ERROR: Dacă dorești să înregistrezi un cont, trebuie să te deloghezi mai întâi (folosește comanda \"logout\")." << "\n" << "\033[0m";
        return;
    }

    // the username and password are both valid,
    // send register request to the server
    
    // initialization of the fields
    string host, url, content_type;

    host = ip;
    url = "/api/v1/tema/auth/register";
    content_type = "application/json";

    json payload;
    payload["username"] = username;
    payload["password"] = password;

    // initialize empty authorization string and empty cookies vector,
    // just for passing it as parameter
    string authorization;
    vector<pair<string, string>> cookies;

    string request = compute_post_request(host, url, content_type, payload, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    switch (response_status_code) {
        case HTTP_CREATED_CODE:
        {
            cout << "\033[32m" << "SUCCESS: Utilizator înregistrat cu succes!\n" << "\033[0m";
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the cookie */
    if (response != NULL) {
        free(response);
    }
}

// Function that logs in an user
// "client_cookie" is empty if the client does not have a cookie
string handle_login(string ip, unsigned short port, string client_cookie) {
    string username, password;
    bool username_is_ok = true;

    cout << "\033[34m" << "username=" << "\033[0m";
    getline(cin, username);
    if (!username_is_valid(username)) {
        username_is_ok = false;
    }

    cout << "\033[34m" << "password=" << "\033[0m";
    getline(cin, password);
    if (!password_is_valid(password) || !username_is_ok) {
        cout << "\033[31m" << "ERROR: username sau/si password au un format incorect!\n" << "\033[0m";
        return "";
    }

    if (!client_cookie.empty()) {
        cout << "\033[31m" << "ERROR: Ești deja autentificat! Mai întâi folosește comanda \"logout\" pentru a te de-autentifica." << "\n" << "\033[0m";
        // return client's cookie
        return client_cookie;
    }

    // the username and password are both valid,
    // send login request to the server

    // initialization of the fields
    string host, url, content_type;

    host = ip;
    url = "/api/v1/tema/auth/login";
    content_type = "application/json";

    json payload;

    payload["username"] = username;
    payload["password"] = password;

    // initialize empty authorization string and empty cookies vector,
    // just for passing it as parameter
    string authorization;
    vector<pair<string, string>> cookies;

    string request = compute_post_request(host, url, content_type, payload, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    string cookie;

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            /* extract session id cookie from the HTTP response body */
            cout << "\033[32m" << "SUCCESS: Utilizatorul s-a logat cu succes!\n" << "\033[0m";
            cookie = get_cookie(response);
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the cookie */
    if (response != NULL) {
        free(response);
    }

    return cookie;
}

// Function that returns an access token for accessing the library
string handle_enter_library(string ip, unsigned short port, string cookie) {
    // initialization of the fields
    string host, url;

    host = ip;
    url = "/api/v1/tema/library/access";

    // initialize the vector of cookies
    vector<pair<string, string>> cookies;

    // initialize empty vector of query parameters;
    // just for passing as parameter
    vector<pair<string, string>> query_params;

    // add only the given cookie to the vector
    cookies.push_back(make_pair("connect.sid", cookie));

    // initialize empty authorization
    string authorization;

    string request = compute_get_request(host, url, query_params, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            /* extract access token from the HTTP response body */
            char* extracted_json = basic_extract_json_response(response);

            if (extracted_json != NULL) {
                json json_response = json::parse(extracted_json);
                json_response["token"].get_to(authorization);
                cout << "\033[32m" << "SUCCESS: Utilizatorul are acces la biblioteca.\n" << "\033[0m";
            } else {
                cout << "\033[31m" << "ERROR: Nu a fost returnat un obiect de tip json!" << "\n" << "\033[0m";
            }
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the access token */
    if (response != NULL) {
        free(response);
    }

    return authorization;
}

// Function that prints all the books from the library
void handle_get_books(string ip, unsigned short port, string authorization) {
    // initialization of the fields
    string host, url;

    host = ip;
    url = "/api/v1/tema/library/books";

    // initialize empty cookies and query_params vectors
    // just for passing as parameters
    vector<pair<string, string>> cookies;
    vector<pair<string, string>> query_params;

    string request = compute_get_request(host, url, query_params, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    string books_json;

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            /* extract books from the HTTP response body */
            char* extracted_json_books = basic_extract_json_books(response);

            if (extracted_json_books != NULL) {
                json json_response = json::parse(extracted_json_books);
                books_json = json_response.dump(4);
                cout << "\033[32m" << "SUCCESS:\n" << books_json << "\n\033[0m";
            } else {
                cout << "\033[31m" << "ERROR: Nu a fost returnat un obiect de tip json!" << "\n" << "\033[0m";
            }
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the access token */
    if (response != NULL) {
        free(response);
    }
}

// Function that prints the details of a book, given its id
void handle_get_book(string ip, unsigned short port, string authorization) {
    string id_string;
    int id;
    cout << "\033[34m" << "id=" << "\033[0m";
    getline(cin, id_string);

    if (authorization.empty()) {
        cout << "\033[31m" << "ERROR: Trebuie să intri mai întâi în bibliotecă pentru a accesa cărțile tale! (folosind comanda \"enter_library\")" << "\n" << "\033[0m";
        return;
    }

    if (book_id_is_valid(id_string)) {
        // remove the 0's from the front of the id, in case there are any of them
        while (id_string[0] == '0') {
            id_string.erase(0, 1);
            if (id_string.empty()) {
                break;
            }
        }
        if (id_string.empty()) {
            // all the digits are 0's, so the book id is 0
            id = 0;
        } else {
            id = stoi(id_string);
        }
    } else {
        cout << "\033[31m" << "ERROR: id-ul cărții are un format incorect!\n" << "\033[0m";
        return;
    }

    // initialization of the fields
    string host, url;

    host = ip;
    url = "/api/v1/tema/library/books/" + to_string(id);

    // initialize empty cookies and query_params vectors
    // just for passing as parameters
    vector<pair<string, string>> cookies;
    vector<pair<string, string>> query_params;

    string request = compute_get_request(host, url, query_params, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    string requested_book_json;

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            /* extract books from the HTTP response body */
            char* extracted_json_books = basic_extract_json_response(response);

            if (extracted_json_books != NULL) {
                json json_response = json::parse(extracted_json_books);
                requested_book_json = json_response.dump(4);
                cout << "\033[32m" << "SUCCESS:\n" << requested_book_json << "\n\033[0m";
            } else {
                cout << "\033[31m" << "ERROR: Nu a fost returnat un obiect de tip json!" << "\n" << "\033[0m";
            }
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the access token */
    if (response != NULL) {
        free(response);
    }
}

// Function that adds a book in the user's library
void handle_add_book(string ip, unsigned short port, string authorization) {
    string title, author, genre, publisher, page_count_string;
    int page_count;

    string format_errors = "";

    cout << "\033[34m" << "title=" << "\033[0m";
    getline(cin, title);
    if (!book_title_is_valid(title)) {
        format_errors = format_errors + "ERROR: Format incorect pentru \"title\"!\n";
    }

    cout << "\033[34m" << "author=" << "\033[0m";
    getline(cin, author);
    if (!book_author_is_valid(author)) {
        format_errors = format_errors + "ERROR: Format incorect pentru \"author\"!\n";
    }

    cout << "\033[34m" << "genre=" << "\033[0m";
    getline(cin, genre);
    if (!book_genre_is_valid(genre)) {
        format_errors = format_errors + "ERROR: Format incorect pentru \"genre\"!\n";
    }

    cout << "\033[34m" << "publisher=" << "\033[0m";
    getline(cin, publisher);
    if (!book_publisher_is_valid(publisher)) {
        format_errors = format_errors + "ERROR: Format incorect pentru \"publisher\"!\n";
    }

    cout << "\033[34m" << "page_count=" << "\033[0m";
    getline(cin, page_count_string);
    if (!book_page_count_is_valid(page_count_string)) {
        format_errors = format_errors + "ERROR: Tip de date incorect pentru numarul de pagini!\n";
    }

    if (authorization.empty()) {
        cout << "\033[31m" << "ERROR: Trebuie să intri mai întâi în bibliotecă pentru a adauga cărți în bibliotecă! (folosind comanda \"enter_library\")" << "\n" << "\033[0m";
        return;
    }

    if (!format_errors.empty()) {
        // at least one field contains format erros, so return error message
        cout << "\033[31m" + format_errors + "\033[0m";
        return;
    }

    // remove the 0's from the front of the page_count, if any
    while (page_count_string[0] == '0') {
        page_count_string.erase(0, 1);
    }
    if (page_count_string.empty()) {
        // all the digits are 0's, so the book page count is 0
        page_count = 0;
    } else {
        // convert string page count to integer
        page_count = stoi(page_count_string);
    }

    // the username and password are both valid,
    // send login request to the server

    // initialization of the fields
    string host, url, body_data, content_type;

    host = ip;
    url = "/api/v1/tema/library/books";
    content_type = "application/json";

    json payload;

    payload["title"] = title;
    payload["author"] = author;
    payload["genre"] = genre;
    payload["publisher"] = publisher;
    payload["page_count"] = page_count;

    // initialize empty cookies vector,
    // just for passing it as parameter
    vector<pair<string, string>> cookies;

    string request = compute_post_request(host, url, content_type, payload, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    string cookie;

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            /* extract session id cookie from the HTTP response body */
            cout << "\033[32m" << "SUCCESS: Utilizatorul a adaugat cartea cu succes!\n" << "\033[0m";
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the cookie */
    if (response != NULL) {
        free(response);
    }
}

// Function that deletes a book from the user's library
void handle_delete_book(string ip, unsigned short port, string authorization) {
    string id_string;
    int id;
    cout << "\033[34m" << "id=" << "\033[0m";
    getline(cin, id_string);

    if (book_id_is_valid(id_string)) {
        // remove the 0's from the front of the id, in case there are any of them
        while (id_string[0] == '0') {
            id_string.erase(0, 1);
        }
        if (id_string.empty()) {
            // all the digits are 0's, so the book id is 0
            id = 0;
        } else {
            id = stoi(id_string);
        }
    } else {
        cout << "\033[31m" << "ERROR: id-ul cărții are un format incorect!\n" << "\033[0m";
        return;
    }

    if (authorization.empty()) {
        cout << "\033[31m" << "ERROR: Trebuie să intri mai întâi în bibliotecă pentru a șterge cărți din bibliotecă! (folosind comanda \"enter_library\")" << "\n" << "\033[0m";
        return;
    }

    // initialization of the fields
    string host, url, content_type;

    host = ip;
    url = "/api/v1/tema/library/books/" + to_string(id);
    content_type = "application/json";

    // initialize empty cookies and query_params vectors
    // just for passing as parameters
    vector<pair<string, string>> cookies;
    vector<pair<string, string>> query_params;

    string request = compute_delete_request(host, url, query_params, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    string cookie;

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            /* extract session id cookie from the HTTP response body */
            cout << "\033[32m" << "SUCCESS: Cartea cu id " << id << " a fost ștearsă cu succes!\n" << "\033[0m";
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the cookie */
    if (response != NULL) {
        free(response);
    }
}

// Function logs out the user
string handle_logout(string ip, unsigned short port, string cookie) {
    // initialization of the fields
    string host, url;

    host = ip;
    url = "/api/v1/tema/auth/logout";

    // initialize empty cookies vector
    vector<pair<string, string>> cookies;
    // add only the given cookie to the vector
    cookies.push_back(make_pair("connect.sid", cookie));

    // initialize empty authorization value and empty query_params vector
    // just for passing as parameters
    string authorization;
    vector<pair<string, string>> query_params;

    string request = compute_get_request(host, url, query_params, cookies, authorization);

    int sockfd = open_connection(ip.c_str(), port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request.c_str());

    char *response = receive_from_server(sockfd);
    close(sockfd);

    unsigned short response_status_code = get_response_code(response);

    string books_json;

    switch (response_status_code) {
        case HTTP_OK_CODE:
        {
            cout << "\033[32m" << "SUCCESS: Utilizatorul s-a delogat cu succes\n" << "\033[0m";
            break;
        }
        case HTTP_REQUEST_TIMEOUT_CODE:
        {
            cout << "\033[31m" << "ERROR: A fost închisă conexiunea la server din cauza că serverul nu a primit request-ul în timpul acordat primirii acestuia (Timeout).\nTe rog să introduci din nou comanda.\n" << "\033[0m";
            break;
        }
        default:
        {
            /* handle all the other types of errors */
            handle_response_errors(response);
            break;
        }
    }

    /* free allocated memory and return the access token */
    if (response != NULL) {
        free(response);
    }

    return books_json;
}

int main(int argc, char *argv[])
{
    // for opening a connection
    string ip;
    unsigned short port;

    // set the ip and port of the server
    ip = "34.246.184.49";
    port = 8080;

    // initialize the variables
    string cookie = "";
    string authorization = "";
    string client_input = "";

    while (true) {
        // receiving input from STDIN
        getline(cin, client_input);

        if (client_input.compare("register") == 0) {
            handle_register(ip, port, cookie);
        }
        else if (client_input.compare("login") == 0) {
            cookie = handle_login(ip, port, cookie);
        }
        else if (client_input.compare("enter_library") == 0) {
            if (!cookie.empty()) {
                if (!authorization.empty()) {
                    cout << "\033[31m" << "ERROR: Ești deja în bibliotecă!\n\033[0m";
                } else {
                    authorization = handle_enter_library(ip, port, cookie);
                }
            } else {
                cout << "\033[31m" << "ERROR: Trebuie să te loghezi mai întai înainte de accesa biblioteca! (folosind comanda \"login\")" << "\n" << "\033[0m";
            }
        }
        else if (client_input.compare("get_books") == 0) {
            if (!authorization.empty()) {
                handle_get_books(ip, port, authorization);
            } else {
                cout << "\033[31m" << "ERROR: Trebuie să intri mai întâi în bibliotecă pentru a accesa cărțile tale! (folosind comanda \"enter_library\")" << "\n" << "\033[0m";
            }
        }
        else if (client_input.compare("get_book") == 0) {
            handle_get_book(ip, port, authorization);
        }
        else if (client_input.compare("add_book") == 0) {
            handle_add_book(ip, port, authorization);
        }
        else if (client_input.compare("delete_book") == 0) {
            handle_delete_book(ip, port, authorization);
        }
        else if (client_input.compare("logout") == 0) {
            if (!cookie.empty()) {
                handle_logout(ip, port, cookie);
                /* reset cookie and authorization */
                cookie = "";
                authorization = "";
            } else {
                cout << "\033[31m" << "ERROR: Nu sunteți logat în cont! (folosește comanda \"login\" pentru a te loga)" << "\n" << "\033[0m";
            }
        }
        else if (client_input.compare("exit") == 0) {
            break;
        }
        else {
            cout << "\033[31m" << "ERROR: Comanda nu a fost recunoscuta!\nComenzile acceptate sunt: register, login, enter_library, get_books, get_book, add_book, delete_book, logout, exit.\n" << "\033[0m";
        }
    }
    return 0;

}
