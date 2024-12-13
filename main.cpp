#include <iostream>
#include <fstream>
#include <filesystem>
#include "libs/json.hpp"
#include <thread>

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

std::string displayEntity(const json &entity)
{
    std::cout << "Client ID: " << entity["client_id"] << std::endl;
    std::cout << "Photobooth ID: " << entity["photobooth_id"] << std::endl;
    std::cout << "Session ID: " << entity["session_id"] << std::endl;
    std::cout << "Captured Time: " << entity["captured"] << std::endl;
    std::cout << "Image URL: " << entity["url"] << std::endl;
    std::cout << "Media Type: " << entity["media_type"] << std::endl;
    std::cout << "Approval Status: " << entity["approval_status"] << std::endl;
    std::cout << std::endl;

    return entity["photobooth_id"];
}

void removeEntityFromQueue(const std::string &filePath, const std::string &photoboothId)
{
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open())
    {
        throw std::runtime_error("Could not open the JSON file.");
    }

    json queueData;
    inputFile >> queueData;
    inputFile.close();

    // Find and display the entity
    auto it = std::find_if(queueData.begin(), queueData.end(), [&](const json &entry)
    {
        return entry["photobooth_id"] == photoboothId; // Assuming session_id is unique
    });

    if (it != queueData.end())
    {
        queueData.erase(it);

        std::ofstream outputFile(filePath);
        if (!outputFile.is_open())
        {
            throw std::runtime_error("Could not open the JSON file for writing.");
        }
        outputFile << queueData.dump(4);
        outputFile.close();

        std::cout << "Entity removed successfully.\n";
    }
    else
    {
        std::cout << "Entity with session ID " << photoboothId << " not found.\n";
    }
}

void uploadImageToFirebase(const std::string &filePath)
{
    std::cout << "Uploading media (" << filePath << ") to firebase storage.\n";
    // // Initialize Firebase storage
    // firebase::storage::StorageReference storageRef = firebase::storage::StorageReference();

    // // Upload file to Firebase Storage
    // // This part of the code depends on the Firebase SDK for Windows
    // storageRef.PutFile(filePath.c_str(), "uploads/my_image.jpg", [](firebase::storage::StorageReference& ref, firebase::storage::Metadata metadata) {
    //     // Handle success or failure
    //     OutputDebugString(L"Image uploaded successfully.\n");
    // });
    std::cout << "Media uploaded successfully.\n";
}

void backgroundUploadTask(const std::string &documentDirPath)
{
    std::cout << "Starting thread with document directory " << documentDirPath << std::endl;
    fs::path documentsPath = fs::path(documentDirPath);

    std::string settingFilePath = (documentsPath / "photobooth_data.json").string();
    json settingsData = readJsonFromFile(settingFilePath);

    std::string photosPath = settingsData["fileDownloadLocation"];

    std::cout << "Collecting Photos from " << photosPath << std::endl;

    std::string queuePath = (documentsPath / "upload_queue.json").string();

    int i = 0;
    while (true)
    {
        std::cerr << "Executing# " << ++i << std::endl;
        try
        {
            json queueData = readJsonFromFile(queuePath);

            std::cout << queueData.size() << " media items founded." << std::endl;

            for (const auto &entry : queueData)
            {
                std::string photoboothId = displayEntity(entry);
                std::string mediaFilePath = entry["url"];

                uploadImageToFirebase(mediaFilePath);

                removeEntityFromQueue(queuePath, photoboothId);
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

int main()
{
    std::thread uploadThread(backgroundUploadTask, "C:\\Users\\Muhammad Hassan\\OneDrive\\Documents");
    if (uploadThread.joinable())
    {
        uploadThread.detach();
    }
    else
    {
        std::cerr << "Failed to create upload thread." << std::endl;
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
    return 0;
}