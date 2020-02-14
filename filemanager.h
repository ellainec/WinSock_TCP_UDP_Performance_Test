#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <fstream>
#include <QDebug>

class FileManager
{
public:
    FileManager();
    static void clearFile() {
        std::ofstream writeFile;
        writeFile.open("D:/CST/4985/Assignment2/Assignment2/WriteTo.txt", std::ofstream::out | std::ofstream::trunc);
        writeFile.close();
    }
    static void printToFile(std::string str);
    static void readFromFile(char* buffer, int, int);
    static int getFileSize();
};

#endif // FILEMANAGER_H
