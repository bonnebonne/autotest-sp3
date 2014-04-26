/*
 *  File: main.cpp
 *  Authors-Sprint1: Joseph Lillo, Dan Nix, Lisa Woody
 *  Authors-Sprint2: Ryan Brown, Kelsey Bellew, Ryan Feather
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
    int i;
    vector<string> cpps;
    string class_dir;
    TestSuite t;

    //If they did not provide the minimum set of args
    if( argc < 3 )
    {
        cout << "Usage: ./tester [-g|-r] [directory] | [-p]" << endl;
        cout << "       -g: Generate test cases from \"golden\" .cpp" << endl;
        cout << "       -r: Run tests on student programs." << endl;
        cout << " 		-p: Profile student code." << endl;
        return -1;
    }

    class_dir = argv[2];
    if(chdir(argv[2]))
    {
        cout << "Failed to change to directory: " << class_dir;
    }


    string profile_flag = "";
    if(argc == 4)
    {
        profile_flag = argv[3];
        if(profile_flag == "-p")
            t.profiling = true;
    }

    //Choose one of two modes. [R]unning tests or [G]enerating tests.
    string flag = argv[1];
    if(flag == "-g")
    {
        //Call test generation function
        t.helper_func();
    }
    else if(flag == "-r")
    {
        //fill "cpps" with the name of every .cpp to be ran
        t.dirCrawl(".cpp", ".", cpps);


        //loop through every .cpp and run it
        for(i=0; i<(int)cpps.size(); i++)
        {
            //Excludes the "golden" .cpp from being evaluated
            if(count(cpps.at(i).begin(), cpps.at(i).end(), '/') > 1)
            {
                //t.initTest(argv[1],".tst",".ans");
                t.initTest( cpps.at(i) ,".tst",".ans");
                t.runTests();
                t.outputLogFile();
            }
        }
        //end for loop

        t.createSummary();
    }
    else
    {
        cout << "Unrecognized command line option. [-g|-r]" << endl;
        return -2;
    }
    return 0;
}



