#include "filemanager.h"

FileManager::FileManager()
{

}

void FileManager::printToFile(std::string str){
    std::ofstream writeFile;
    writeFile.open("\WriteTo.txt", std::ios_base::app);
    if(!writeFile.is_open()) {
        qDebug() << "open file failed";
    }
    writeFile << str;
    writeFile.close();
}

void FileManager::readFromFile(char* buffer, int beg, int length) {
    std::ifstream readFile;
    readFile.open("/ReadFrom.txt", std::ifstream::binary);
    if(readFile.is_open()) {
        readFile.seekg(beg);
        readFile.read(buffer, length);
    } else {
        qDebug() << "write failed";
    }
}

int FileManager::getFileSize(){
    std::ifstream f;
    f.open("test.txt", std::ifstream::binary);
    if(f.is_open()) {
        f.seekg(0, f.end);
        return f.tellg();
    }
    return -1;
}
