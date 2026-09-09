#pragma once

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>

struct APIResponse {
    long status;
    std::string body;
};

enum class HTTPMethod {
    GET,
    POST,
    DELETE,
    PUT,
    PATCH,
    HEAD,
};

class APIClient {
    public:
        APIClient();
        ~APIClient();

        APIResponse get(const std::string& url);
        APIResponse request(HTTPMethod method, const std::string& url, const std::string& body = "");

        void append_header(std::string header);
    
    private:
        CURL* _curl;
        std::vector<std::string> _headers;

        curl_slist* get_curl_headers();
};