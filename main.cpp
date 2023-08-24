#include <iostream>
#include "curl/include/curl/curl.h"


using namespace std;




// Callback function to handle the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* user_data) {
    size_t total_size = size * nmemb;
    std::string* response = static_cast<std::string*>(user_data);
    response->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int main() {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.100.72:5000/hello");

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "cURL request error: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Response: " << response << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    return 0;
}

