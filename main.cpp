#include "mainwindow.h"

#include <QApplication>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();
    return 1;
  }
