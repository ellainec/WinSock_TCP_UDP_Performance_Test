#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <fstream>
#include <QDebug>

class FileManager
{
public:
    FileManager() {};
    static void clearFile() {
        std::ofstream writeFile;
        writeFile.open("\WriteTo.txt", std::ofstream::out | std::ofstream::trunc);
        writeFile.close();
    }
    static void printToFile(std::string str) {
        std::ofstream writeFile;
        writeFile.open("\WriteTo.txt", std::ios_base::app);
        if(!writeFile.is_open()) {
            qDebug() << "open file failed";
        }
        writeFile << str;
        writeFile.close();
    };
};

#endif // FILEMANAGER_H
