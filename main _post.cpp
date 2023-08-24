#include <iostream>
#include "curl/include/curl/curl.h"
#include <string>
#include "nlohmann/json.hpp"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Curl initialization failed." << std::endl;
        return 1;
    }

    std::string url = "http://127.0.0.1:5000/dicom1";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the POST data (uploading the image)
    struct curl_httppost* post = NULL;
    struct curl_httppost* last = NULL;
    curl_formadd(&post, &last, CURLFORM_COPYNAME, "image", CURLFORM_FILE, "1.dcm", CURLFORM_END);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "HTTP request failed: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    curl_easy_cleanup(curl);
    curl_formfree(post);

    // Process the response...

    nlohmann::json jsonResponse = nlohmann::json::parse(response);

    // Extract values from the JSON response
    std::string seriesInstanceUID = jsonResponse["SeriesInstanceUID:"].get<std::string>();
    std::string studyInstanceUID = jsonResponse["StudyInstanceUID:"].get<std::string>();
    std::string sopInstanceUID = jsonResponse["SOPInstanceUID:"].get<std::string>();

    // Extract boxes and scores as JSON arrays
    std::vector<std::vector<double>> boxes = jsonResponse["boxes"].get<std::vector<std::vector<double>>>();
    std::vector<double> scores = jsonResponse["scores"].get<std::vector<double>>();

    // Print extracted values
    std::cout << "SeriesInstanceUID: " << seriesInstanceUID << std::endl;
    std::cout << "StudyInstanceUID: " << studyInstanceUID << std::endl;
    std::cout << "SOPInstanceUID: " << sopInstanceUID << std::endl;

    // Process the JSON data...
    // Assuming boxes and scores are arrays of arrays/doubles
    for (const auto& box : boxes) {
        // Process each box array
        for (double value : box) {
            std::cout << "Box Value: " << value << std::endl;
        }
    }

    for (double score : scores) {
        std::cout << "Score: " << score << std::endl;
    }

    return 0;
}
