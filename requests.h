#ifndef _REQUESTS_
#define _REQUESTS_

#define HTTP_OK_CODE 200
#define HTTP_CREATED_CODE 201
#define HTTP_REQUEST_TIMEOUT_CODE 408

#include <vector>
#include "json.hpp"
using namespace nlohmann::json_abi_v3_11_3;
using namespace std;

// Function that computes a "GET" HTTP request with the provided information ("query_params" or/and "cookies" can be empty if not needed)
string compute_get_request(string host, string url, vector<pair<string, string>>& query_params,
                            vector<pair<string, string>>& cookies, string authorization);

// Function that computes a "POST" HTTP request with the provided information ("cookies" can be empty if not needed)
string compute_post_request(string host, string url, string content_type, json payload,
							vector<pair<string, string>>& cookies, string authorization);

// Function that computes a "DELETE" HTTP request with the provided information ("query_params" or/and "cookies" may be empty)
string compute_delete_request(string host, string url, vector<pair<string, string>>& query_params,
                            vector<pair<string, string>>& cookies, string authorization);
#endif
