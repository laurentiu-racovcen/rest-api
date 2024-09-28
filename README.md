# **Web Client. REST API Communication.**

>This project consists in implementing a HTTP client in C++ that interacts with a REST API exposed by a server through the HTTP protocol.
The client can interact with the server via the following keyboard commands: `register`, `login`, `enter_library`, `get_books`,
`get_book`, `add_book`, `delete_book`, `logout`, `exit`.

In this way, a user can easily manage the books in their library, with the ability to
add/remove books to/from the library and view their details.


To realize the given requirements, I've implemented helper functions in the following files: `"utils.h"`,
`"input_restrictions.h"`, `"requests.h"` (implemented in `requests.cpp`).

For `JSON` content processing, I used the [`nlohmann`](https://github.com/nlohmann/json) library (the "json.hpp" file).

## **Table of contents**

1. ["main" function](#main-function)
2. [Functions for processing user commands](#functions-for-processing-user-commands)
3. [Functions in the "requests.cpp" file](#functions-in-the-requestscpp-file)
4. [Functions in the "utils.h" file](#functions-in-the-utilsh-file)
5. [Functions in the "input_restrictions.h" file](#functions-in-the-input_restrictionsh-file)

## **"main" function**

**1.** Initialize the `IP` and `Port` where the REST API is exposed by the server.

**2.** Initialize the variables `cookie`, `authorization` and `client_input`, required
for the user to interact with the server.

**3.** In the `while` loop that follows, it continuously waits for commands from the user (from `stdin`), which are then processed. When the user gives the `"exit"` command, the program exits.

## **Functions for processing user commands**

- `"handle_register"` - registers a user based on a username and a password. This function sends a `POST` request to `/api/v1/tema/auth/register` with the entered credentials. If the credentials have an incorrect format, an error is returned, and the request is not sent to the server.

- `"handle_login"` - authenticates a user based on a username and password. This function sends a `POST` request to `/api/v1/tema/auth/login` 
with the entered credentials and returns a `cookie` set by the server for the client. If the 
credentials have an incorrect format, an error is returned and the request is not sent to the server.

- `"handle_enter_library"` - requests access to the user's library. This function sends a `GET` request to `/api/v1/tema/library/access` with the 
`cookie` received from the server and returns an `access token` (authorization) set by the server for the client.
Library access is only allowed if the user has a valid `cookie` provided by the server.
If the user does not have a `cookie` to prove the identity, the user is required to log in.

- `"get_books"` - displays brief details of all books in the user's library. This function sends a `GET` request to `/api/v1/tema/library/books` with the
`access token` (authorization) received from the server.

- `"get_book"` - displays the details of a book from the user's library (given its id). This function sends a `GET` request to `/api/v1/tema/library/books/:bookId` with the 
`access token` (authorization) received from the server, where `:bookId` is the id of the requested book.

- `"add_book"` - adds a new book to the user's library (details about the book are given). This function sends a `POST` request to `/api/v1/tema/library/books` with the 
`access token` (authorization) received from the server.

- `"delete_book"` - delete a book from the user's library (given its id). This function sends a `DELETE` request to `/api/v1/tema/library/books/:bookId` with the
`access token` (authorization) received from the server.

- `"logout"` - performs user logout. This function sends a `GET` request to `/api/v1/tema/auth/logout` with the
`cookie` received from the server.
The user must already be logged into the system. If the user is not logged in, an error is returned.

<b> The functions `get_books`, `get_book`, `add_book`, `delete_book` require a valid `access token` (authorization) provided by the server for the client.
If the user does not have an `access token` to prove identity, the user is asked to enter the library (using the `enter_library` command). </b>

## **Functions in the "requests.cpp" file**

- `"compute_cookie_header"` - generates the `"Cookie"` header of an HTTP Request,
based on a vector of key-value pairs (given as a parameter) of cookies.
- `"compute_authorization_header"` - generates the `"Authorization"` header of an HTTP Request,
based on an access token (authorization) received as a string parameter.
- `"compute_query_params"` - generates a URL query for an HTTP Request,
based on a vector of key-value pairs (given as argument) of the query parameters.
- `"compute_get_request"` - generates a `GET HTTP Request` as a string, which contains the details received as parameters.
- `"compute_post_request"` - generates a `POST HTTP Request` as a string, containing the details received as parameters.
- `"compute_delete_request"` - generates a `DELETE HTTP Request` as a string, containing the details received as parameters.

## **Functions in the "utils.h" file**

- `"get_cookie"` - function that returns the value of the first `cookie` in the `"Cookie` header of an `HTTP Response`.
- `"get_response_code"` - function that returns the `HTTP Response Code` (as `int`) of a `HTTP Response` as a string.

## **Functions in the "input_restrictions.h" file**

- `"username_is_valid"` - checks if a username is valid.
- `"password_is_valid"` - checks if a password is valid.
- `"book_id_is_valid"` - checks if a book id is valid.
- `"book_title_is_valid"` - checks if a book title is valid.
- `"book_author_is_valid"` - checks if a book author is valid.
- `"book_genre_is_valid"` - checks if a book genre is valid.
- `"book_publisher_is_valid"` - checks if a book publisher is valid.
- `"book_page_count_is_valid"` - checks if the page count of the book is valid.
