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
    //if no directory is given via the command line, use current directory
    if ( argc < 2 )
    {
      string class_dir;
      class_dir = argv[1];
      class_dir = class_dir.substr(0,class_dir.find("."));
      size_t found = class_dir.find_last_of("/\\");
      class_dir = class_dir.substr(0,found+1);
      
      TestSuite t;
      t.initTest(class_dir,".tst",".ans");
      t.runTests();
      t.outputLogFile();;
    }
    //else use the one passed via command line
    else
    {
      TestSuite t;
      t.initTest(argv[1],".tst",".ans");
      t.runTests();
      t.outputLogFile();
    }
    return 0;
}

