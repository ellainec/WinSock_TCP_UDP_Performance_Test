#include "mainwindow.h"

#include <QApplication>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: main.cpp
--
-- PROGRAM: main
--
-- FUNCTIONS:
-- in main (int argc, char *argv[])
--
-- DATE: February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- NOTES:
-- Entry point of the program. This will start up the main window that the user will interact with.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();
    return 1;
  }
