#include <iostream>
#include <fstream>
#include <filesystem>
#include "libs/json.hpp"

// Namespace for convenience
using json = nlohmann::json;
namespace fs = std::filesystem;

json readJsonFromFile(const std::string &filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        throw std::runtime_error("Could not open the JSON file: " + filePath);
    }

    json jsonData;
    file >> jsonData;
    return jsonData;
}

void displayEntity(const json &entity)
{
    std::cout << "Client ID: " << entity["client_id"] << std::endl;
    std::cout << "Photobooth ID: " << entity["photobooth_id"] << std::endl;
    std::cout << "Session ID: " << entity["session_id"] << std::endl;
    std::cout << "Captured Time: " << entity["captured"] << std::endl;
    std::cout << "Image URL: " << entity["url"] << std::endl;
    std::cout << "Media Type: " << entity["media_type"] << std::endl;
    std::cout << "Approval Status: " << entity["approval_status"] << std::endl;
    std::cout << std::endl;
}

int main()
{
    try
    {
        fs::path documentsPath = fs::path(std::getenv("HOME")) / "Documents";

        std::string settingFilePath = (documentsPath / "photobooth_data.json").string();
        json settingsData = readJsonFromFile(settingFilePath);

        std::string photosPath = settingsData["fileDownloadLocation"];
        std::cout << "File Download Location: " << photosPath << std::endl;

        std::string queuePath = (documentsPath / "upload_queue.json").string();
        json queueData = readJsonFromFile(queuePath);

        std::cout << "Queue Data: " << queueData << std::endl;

        for (const auto &entry : queueData)
        {
            displayEntity(entry);
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}