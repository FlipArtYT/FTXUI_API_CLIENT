#include <curl/curl.h>
#include "../include/simplerapiclient.hpp"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

APIClient::APIClient() {
    _curl = curl_easy_init();
    if (!_curl) {
        std::cerr << "APIClient: curl_easy_init() failed\n";
    }
}

APIClient::~APIClient() {
    curl_easy_cleanup(_curl);
}

APIResponse APIClient::get(const std::string& url) {
    if (!_curl) {
        return APIResponse { 0L, "" };
    }

    CURLcode result;
    long status_code = 0;
    std::string readBuffer;
    curl_slist* curl_headers = get_curl_headers();

    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, curl_headers);

    result = curl_easy_perform(_curl);

    curl_slist_free_all(curl_headers);

    if (result != CURLE_OK) {
        // std::cerr << "curl_easy_perform() failed: "
        //         << curl_easy_strerror(result) 
        //         << "\n";

        return APIResponse {
            0L,
            curl_easy_strerror(result)
        };
    }

    CURLcode info_result = curl_easy_getinfo(
        _curl,
        CURLINFO_RESPONSE_CODE,
        &status_code
    );

    if (info_result != CURLE_OK) {
        std::cerr << "curl_easy_getinfo failed: "
                << curl_easy_strerror(info_result)
                << '\n';
    }

    return APIResponse {
        status_code,
        readBuffer
    };
}

APIResponse APIClient::request(HTTPMethod method, const std::string& url, const std::string& body) {
    if (!_curl) {
        return APIResponse { 0L, "" };
    }

    CURLcode result;
    long status_code = 0;
    std::string readBuffer;
    curl_slist* curl_headers = get_curl_headers();

    // Configure API request
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, curl_headers);

    switch (method) {
        case HTTPMethod::GET:
            curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
            break;
        
        case HTTPMethod::POST:
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
            break;
        
        case HTTPMethod::DELETE:
            curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        
        case HTTPMethod::PUT:
            curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body.c_str());
            break;
        
        case HTTPMethod::PATCH:
            curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body.c_str());
            break;

        case HTTPMethod::HEAD:
            curl_easy_setopt(_curl, CURLOPT_NOBODY, 1L);
            break;
    }

    // Perform actual API request
    result = curl_easy_perform(_curl);

    curl_slist_free_all(curl_headers);

    if (result != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: "
                << curl_easy_strerror(result) 
                << "\n";

        return APIResponse {
            0L,
            readBuffer
        };
    }

    // Get response code
    CURLcode info_result = curl_easy_getinfo(
        _curl,
        CURLINFO_RESPONSE_CODE,
        &status_code
    );

    if (info_result != CURLE_OK) {
        std::cerr << "curl_easy_getinfo failed: "
                << curl_easy_strerror(info_result)
                << '\n';
    }

    return APIResponse {
        status_code,
        readBuffer
    };
}

void APIClient::append_header(std::string header) {
    std::cout << "Appending \"" << header << "\" to header\n";
    _headers.push_back(header);
}

curl_slist* APIClient::get_curl_headers() {
    curl_slist* converted_curl_headers = nullptr;

    for (const std::string& header : _headers) {
        converted_curl_headers = curl_slist_append(
            converted_curl_headers,
            header.c_str()
        );
    }

    return converted_curl_headers;
}