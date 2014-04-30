/*
 *  File: main.cpp
 *  Authors-Sprint1: Joseph Lillo, Dan Nix, Lisa Woody
 *  Authors-Sprint2: Ryan Brown, Kelsey Bellew, Ryan Feather
 *	Authors-Sprint3: Anthony Morast, Benjamin Sherman, James Tillma
 *  Date: 4/27/14
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
 *               test as well as code coverage and if enabled profiling info.
 *
 *				Sprint3: Added is a feature that stops a student program if it
 *				takes more than the allowed time set by the user.
 *				String and menu driven test cases can now be automatically
 *				generated.
 * 				Custom diff enabled by the user (presentation errors) that
 *				allows a student program solution to differ slightly from
 * 				the answer
 */

#include "testsuite.h"




int main(int argc, char ** argv)
{
    int i;
    vector<string> cpps;
    string class_dir = "", presentationOpt = "";
    TestSuite t;
    vector<string> spec;
    bool menuTesting;
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
        t.dirCrawl(".spec", ".",spec);

        //if spec file exists, do menu driven testing
        if ((int)spec.size() != 0)
            menuTesting = t.menu_tests(spec[0]);
        //Call test generation function

        if(!menuTesting)
            t.helper_func();
    }
    else if(flag == "-r")
    {
		bool run = true, tests_initialized = false;
		string test_again;
		while(run)
		{
			test_again = "";
			cout << "What is the maximum amount of time you would like a program to run";
			cout << " before it is considered an infinite loop (in seconds)?  ";
			cout << endl;
			cin >> t.allowed_time;

			t.presentationMenu();

			//fill "cpps" with the name of every .cpp to be ran
			t.dirCrawl(".cpp", ".", cpps);


			//loop through every .cpp and run it
			for(i=0; i<(int)cpps.size(); i++)
			{
				//Excludes the "golden" .cpp from being evaluated
				if(count(cpps.at(i).begin(), cpps.at(i).end(), '/') > 1)
				{
					cout << cpps.at(i) << endl;
					//t.initTest(argv[1],".tst",".ans");
					if(!tests_initialized)
						t.initTest( cpps.at(i) ,".tst",".ans");
						
					
						
					t.runTests(cpps.at(i));
					t.outputLogFile();
				}
			}
			//end for loop
			tests_initialized = true;


			t.createSummary();
			while(test_again != "y" && test_again != "n")
			{
				cout << "Do you want to run another test? (y/n)";
				cin >> test_again;
			}
			if(test_again == "n")
				run = false;
		}
		t.cleanDir();
		system ("rm dummy.out");
		system ("rm test_out.klein");
    }
    else
    {
        cout << "Unrecognized command line option. [-g|-r]" << endl;
        return -2;
    }
    return 0;
}



