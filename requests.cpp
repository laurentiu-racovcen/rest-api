#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <vector>
#include <string>
#include "json.hpp"
#include "helpers.h"
#include "requests.h"

using namespace std;

// Function that generates the "Cookie" header of the request, using the given cookies;
// if the cookies vector is empty, no cookies are added
string compute_cookie_header(vector<pair<string, string>>& cookies) {
    if (cookies.empty()) {
        return "";
    }
    string cookie_header = "Cookie: ";

    // add all cookies
    for (size_t i = 0; i < cookies.size(); i++) {
        cookie_header += cookies[i].first + "=" + cookies[i].second + "; ";
    }
    return cookie_header + "\r\n";
}

// Function that generates the "Authorization" header of the request, using the given authorization value;
// if the authorization value is empty, no authorization is added
string compute_authorization_header(string authorization) {
    if (authorization.empty()) {
        return "";
    }
    return "Authorization: Bearer " + authorization + "\r\n";
}

// Function that generates the query of the URL of the request, using the given query parameters;
// if the "query_params" vector is empty, no query params are added
string compute_query_params(vector<pair<string, string>>& query_params) {
    if (query_params.empty()) {
        return "";
    }

    string query = "?";
    // add all query parameters
    for (size_t i = 0; i < query_params.size(); i++) {
        query += query_params[i].first + "=" + query_params[i].second;
        if (i < query_params.size()-1) {
            query += "&";
        }
    }
    return query;
}

string compute_get_request(string host, string url, vector<pair<string, string>>& query_params,
                            vector<pair<string, string>>& cookies, string authorization)
{
   return "GET " + url + compute_query_params(query_params) + " HTTP/1.1" + "\r\n" // write method name, URL, query parameters, protocol type
              + "Host: " + host + "\r\n"                                           // write host
              + compute_cookie_header(cookies)                                     // add cookies (if any)
              + compute_authorization_header(authorization)                        // add authorization (if any)
              + "Connection: keep-alive" + "\r\n"                                  // to avoid the server closing the TCP connection
              + "\r\n";                                                            // add new line at the end of the header
}

string compute_post_request(string host, string url, string content_type, json payload,
                            vector<pair<string, string>>& cookies, string authorization)
{
    return "POST " + url + " HTTP/1.1" + "\r\n"                                   // write method name, URL, query parameters, protocol type
              + "Host: " + host + "\r\n"                                          // write host
              + compute_cookie_header(cookies)                                    // add cookies (if any)
              + compute_authorization_header(authorization)                       // add authorization (if any)
              + "Content-Type: " + content_type + "\r\n"                          // add content type
              + "Content-Length: " + to_string(payload.dump(4).length()) + "\r\n" // add payload (body) length
              + "Connection: keep-alive" + "\r\n"                                 // to avoid the server closing the TCP connection
              + "\r\n"                                                            // add new line at the end of the header
              + payload.dump(4);                                                  // add the actual payload data (body)
}

string compute_delete_request(string host, string url, vector<pair<string, string>>& query_params,
                            vector<pair<string, string>>& cookies, string authorization)
{
    return "DELETE " + url + compute_query_params(query_params) + " HTTP/1.1" + "\r\n" // write method name, URL, query parameters, protocol type
              + "Host: " + host + "\r\n"                                               // write host
              + compute_cookie_header(cookies)                                         // add cookies (if any)
              + compute_authorization_header(authorization)                            // add authorization (if any)
              + "Connection: keep-alive" + "\r\n"                                      // to avoid the server closing the TCP connection
              + "\r\n";                                                                // add new line at the end of the header
}
