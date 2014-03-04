/*
 *  File: main.cpp
 *  Authors: Joseph Lillo, Dan Nix, Lisa Woody
 *  Date: 2/19/14
 *  Class: Software Engineering
 *  Professor: Dr. Logar
 *  Description: Main entry point for software test suite program.
 *               This program allows the user to automatically compile
 *               a c++ program and test it with known input/output cases.
 *               The program is run from the command line with the name of the
 *               c++ file to test. This program will then compile the c++ file
 *               and recursively search child directories for .tst and .ans
 *               files. The program will then save the output from the tested
 *               program in a timestamped directory. This program will also
 *               output a timestamped log file that outlines the results of the
 *               test.
 */

#include "testsuite.h"


int main(int argc, char ** argv)
{
    if ( argc < 2 )
    {
        cout << "Specify program to test." << endl;
        return 1;
    }

    TestSuite t;
    t.initTest(argv[1],".tst",".ans");
    t.runTests();
    t.outputLogFile();

    return 0;
}

