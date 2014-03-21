/*
 *  File: testsuite.h
 *  Authors: Joseph Lillo, Dan Nix, Lisa Woody
 *  Date: 2/19/14
 *  Class: Software Engineering
 *  Professor: Dr. Logar
 *  Description: TestSuite class definition.
 */

#ifndef TESTERSUITE_H
#define TESTERSUITE_H

#include <string>
#include <vector>
#include <dirent.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <sstream>

using namespace std;

/*!
 * \brief This class provides an interface for compiling and testing c++ programs against given test files.
 */
class TestSuite
{
public:

    /*!
     * \brief TestSuite Constructor
     */
    TestSuite ();

    /*!
     * \brief Set the program to compile and test.
     * \param program The name of the program.
     */
    //void setProgram(string program) { testProgram = program; }

    /*!
     * \brief Returns the name of the program that will be tested.
     * \return Name of program.
     */
    //string getTestProgram() const {return testProgram;}

    /*!
     * \brief Initialize a testing session. Compiles the given program and locates all test and answer files.
     * \param program Name of program to test.
     * \param tstExt File extension of test files to search for.
     * \param ansExt File extension of answer files to search for.
     * \return Success or Failure
     */
    bool initTest (string program, string tstExt, string ansExt);

    /*!
     * \brief Reset private member data after completing a test session.
     * \return Success or Failure
     */
    bool reset ();

    /*!
     * \brief Run the test.
     */
    void runTests();

    /*!
     * \brief Output a log file with results of test.
     */
    void outputLogFile();

    void find_students(vector<string> &studentDirs);

    /*!
     * \brief Recursive directory crawl for given file extension.
     * \param targetExt File extension to search for.
     * \param dir Root directory of search.
     * \param dest Vector to store files that meet the search criteria.
     */
    void dirCrawl (string targetExt, string dir, vector<string> & dest);


private:

        /*!
     * \brief Compile a c++ program.
     * \param filename Name of file to compile (c++ files only).
     * \return Success or Failure
     */
    bool compile_code( string filename );

    /*!
     * \brief Run the program against a given test file.
     * \param test_file The test file path for input to the program.
     * \param test_file_name The name of the test file.
     * \return
     */
    bool run_code(string test_file , string test_file_name);

    /*!
     * \brief Compares program output to answer file.
     * \param ans_file File to compare program output to.
     * \return
     */
    bool correct_answer( string ans_file );

    
    /*!
     * \brief Menu to gather required data from user by reference..
     * \param datatype: 1 = ints, 2 = floats.
     * \param number_of_testcases: 1 to 2,147,483,647.
     * \param numbers_per_testcase: 1 to 2,147,483,647.
     * \param min_value: mimimum value of randomly generated numbers.
     * \param max_value: maximum value of randomly generated numbers.
     */
    void menu(int& datatype, int& number_of_testcases, int& numbers_per_testcase,double& min_value,
	double& max_value);
	
    int rand_tests(double max, double min, int num_tests, string goldencpp); //returns 0 for success, -1 for failure
 
	/*!
     * \brief Calls Menu(), finds golden cpp, calls rand_tests.
     */
	void TestSuite::helper_func()
 
    //! Program to test.
    string testProgram;
    //! List of test files.
    vector<string> testFiles;

    //! Test file extension.
    string testExtension;

    //! Answer file extension.
    string answerExtension;

    //! Execution time of program.
    string exeTime;
};

#endif


