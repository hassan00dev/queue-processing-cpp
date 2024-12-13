#include <iostream>
#include <fstream>
#include <filesystem>
#include "libs/json.hpp"

// Namespace for convenience
using json = nlohmann::json;
namespace fs = std::filesystem;

json readJsonFromFile(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open the JSON file: " + filePath);
    }

    json jsonData;
    file >> jsonData;
    return jsonData;
}

int main() {
    try {
        fs::path documentsPath = fs::path(std::getenv("HOME")) / "Documents";

        std::string fileName = "photobooth_data.json";
        std::string jsonFilePath = (documentsPath / fileName).string();

        json jsonData = readJsonFromFile(jsonFilePath);

        std::cout << "JSON Content: " << jsonData.dump(4) << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
