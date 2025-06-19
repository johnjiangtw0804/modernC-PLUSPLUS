#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class FileWrapper {
public:
    FileWrapper(const std::string &path);
    // copy constructor: you have no returns value
    FileWrapper(const FileWrapper& obj) = delete;
    // copy assignment returns an reference
    FileWrapper& operator=(const FileWrapper& obj) = delete;

    // move constructor
    FileWrapper(FileWrapper &&) noexcept = default;
    FileWrapper& operator=(FileWrapper&&) noexcept = default;

    ~FileWrapper();

    std::ofstream &get(); // 提供外部寫入權限

private:
    std::ofstream file;
};

// disable copy and override move

FileWrapper::FileWrapper(const std::string &path)
{
    file.open(path);
    if (file.fail()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::cout << "file is opened" << std::endl;
}

FileWrapper::~FileWrapper()
{
    if (file.is_open()) {
        file.close();
        std::cout << "file is closed nicely" << std::endl;
    }
}

std::ofstream &FileWrapper::get()
{
    return file;
}

int main()
{
    try
    {
        FileWrapper fw("output.txt");
        fw.get() << "Hello from RAII FileWrapper!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << "👋 程式即將結束。" << std::endl;
}
