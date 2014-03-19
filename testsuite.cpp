/*
 *  File: testsuite.cpp
 *  Authors: Joseph Lillo, Dan Nix, Lisa Woody
 *  Date: 2/19/14
 *  Class: Software Engineering
 *  Professor: Dr. Logar
 *  Description: Function implementations for TestSuite class.
 */

#include "testsuite.h"

TestSuite::TestSuite()
{
}

// Initialize the testing session.
bool TestSuite::initTest(string program, string tstExt, string ansExt)
{
    testProgram = program;
    testExtension = tstExt;
    answerExtension = ansExt;

    // Compile Test Programs
    if (!compile_code(program))
    {
        cout << "Could not compile student program: " << program;
        return false;
    }

    // Crawl child directories for test files.
    dirCrawl(tstExt, "./tests", testFiles);

    // Determine timestamp
    time_t rawTime;
    tm * timeInfo;
    char buffer [40];

    time (&rawTime);
    timeInfo = localtime (&rawTime);

    strftime (buffer,40,"%d_%m_%y_%H_%M",timeInfo);
    exeTime = buffer;

    return true;
}

// Clear member data associated with testing session.
bool TestSuite::reset()
{
    testFiles.clear();
    exeTime.clear();
    return true;
}

// Runs program with input from test files in testFiles vector.
void TestSuite::runTests()
{
    int numCorrect = 0, numWrong = 0;
    string name;
    string logName;
    string stored_dir;
    double rate;

    // Create directory to store output files.
    stored_dir = "mkdir tested_output";
    stored_dir += exeTime;
    system( stored_dir.c_str() );

    // Create log file.
    logName = "log";
    logName += exeTime;
    logName += ".log";
    ofstream fout(logName.c_str());
    if (!fout)
    {
        return;
    }

    // Iterate over test files.
    vector<string>::iterator it;
    for ( it = testFiles.begin(); it != testFiles.end() ; it++ )
    {
        // Get test file name without path.
        size_t pos = it->rfind("/");
        if(pos != std::string::npos)
        {
            name = it->substr(pos+1,it->length());
        }

        // Output test file name to log file.
        fout << name;

        // Run program with given test file.
        run_code(*it,name);

        // Determine corresponding answer file.
        string ans = *it;
        ans.replace(ans.end()-4, ans.end(),answerExtension);

        // Output results.
        if (correct_answer(ans))
        {
            numCorrect++;
            fout << ": PASS" << endl;
        }
        else
        {
            numWrong++;
            fout << ": FAIL" << endl;
        }
    }

    // Output pass and fail stats.
    rate = ( numCorrect / (double)(numCorrect + numWrong) ) * 100;
    fout << rate <<  "% CORRECT," << numCorrect << " PASSED," << numWrong << " FAILED";
    fout.close();
}

void TestSuite::outputLogFile()
{
}

// Recursive Directory Crawl
void TestSuite::dirCrawl(string targetExt, string dir, vector<string> &dest)
{
    // Open current directory.
    DIR * proc = opendir( dir.c_str() );

    if (NULL == proc)
    {
        return;
    }

    // Read current directory.
    dirent * entry = readdir(proc);

    do
    {
        // Make recursive calls to sub directories
        if(DT_DIR == entry->d_type)
        {
            string name = entry->d_name;
            if ( "." != name && ".." != name )
            {
                string newDir = dir + "/" + entry->d_name;
                dirCrawl( targetExt, newDir, dest );
            }
        }
        // Watch for files with .tst extension
        else if ( DT_REG == entry->d_type )
        {
            string fileName = entry->d_name;

            size_t extPos = fileName.rfind(".");

            if ( extPos != string::npos )
            {
                string ext = fileName.substr( extPos );
                if ( targetExt == ext )
                {
                    fileName = dir + "/" + fileName;
                    cout << fileName << endl;
                    dest.push_back(fileName);
                }
            }
        }
    }while((entry=readdir(proc)));

    closedir(proc);
}

//Function to compile c++ source code based on filename
bool TestSuite::compile_code( string filename ){

    int i = filename.rfind('.');
    string compile_instruction = "g++ ";
    compile_instruction += filename;
    compile_instruction += " -o ";
    
    compile_instruction += filename.substr(0, i);

    cout << compile_instruction << endl;

    if(!system( compile_instruction.c_str() ))
    {
        return false;
    }
    
    return true;
}

//Function to run c++ souce with redirected input/output
bool TestSuite::run_code( string test_file_path, string test_file_name ){

    //This instruction will run the test program with test_file_path piped in.
    //The output will be piped to test_out.klein and also a file in the
    //timestamped output file directory. The klein file is used for comparing
    //the output to the expected value.
    string run_instruction = "./test_prog < ";
    run_instruction += test_file_path;
    run_instruction += " | tee test_out.klein ./tested_output";
    run_instruction += exeTime;
    run_instruction += "/";
    run_instruction += test_file_name;
    run_instruction += ".tested";

    system( run_instruction.c_str() );

    return true;
}

//Function to do diff on answer file and test program output file
bool TestSuite::correct_answer( string ans_file )
{
    string diff_instruction = "diff test_out.klein ";
    diff_instruction += ans_file;

    return (! system( diff_instruction.c_str() ) );

}

void TestSuite::find_students(vector<string> &studentDirs)
{
    // Open current directory.
    DIR * proc = opendir("." );

    if (NULL == proc)
    {
        return;
    }

    // Read current directory.
    dirent * entry = readdir(proc);

    do
    {
        // Make recursive calls to sub directories
        if(DT_DIR == entry->d_type)
        {
            string name = entry->d_name;
            if ( "." != name && ".." != name && "test" != name )
            {
                studentDirs.push_back(name);            
            }
        }
        
    }while((entry=readdir(proc)));

    closedir(proc);
    
    return;
}

/*Function gathers the required data from the user and returns all of the 
values by reference.*/
void TestSuite::menu(int& autogenerate, int& datatype, int& number_of_testcases,
 int& numbers_per_testcase,double& min_value, double& max_value)
{
  
  cout << "--------------------------------------------" << endl;
  cout << "----------Automated Program Tester----------" << endl;
  cout << "--------------------------------------------" << endl;
  cout << "\n\n" << endl;
  cout << 
  "Would you like to auto-generate test cases? (1 for yes, 2 for no)" << endl;
  cin >> autogenerate;
  while(autogenerate != (1 or 0))
  {
    cout << "\nIncorrect choice input." << endl;
    cout << 
    "Would you like to auto-generate test cases? (1 for yes, 2 for no)" << endl;
    cin >> autogenerate;
  }
  
  //if the user wishes the program to auto-generate test cases, then further
  //parametersmust be gathered from the user
  if(autogenerate == 1)
  {
    //getting data type from user
    cout << "What datatype are the numbers? (1 for ints, 2 for floats)" << endl;
    cin >> datatype;
    while(datatype != (1 or 0))
    {
      cout << "\nIncorrect choice input." << endl;
      cout << 
      "What datatype are the numbers? (1 for ints, 2 for floats)" << endl;
      cin >> datatype;
    }
    
    //getting how many test cases to generate from user
    cout << 
    "\nHow many test cases would you like generated?" << 
    "\n(Number between 1 and 2,147,483,647)" << endl;
    cin >> number_of_testcases;
    while((number_of_testcases < 1) or (number_of_testcases > 2147483647))
    {
      cout << "\nHow many test cases would you like generated?" << 
      "\n(Number between 1 and 2,147,483,647)" << endl;
      cin >> number_of_testcases;
    }
    
    //getting how many numbers to generate per test case from user
    cout << 
    "\nHow many random numbers would you like in each test case?" << 
    "\n(Number between 1 and 2,147,483,647)" << endl;
    cin >> numbers_per_testcase;
    while((numbers_per_testcase < 1) or (numbers_per_testcase > 2147483647))
    {
      cout << 
      "\nHow many random numbers would you like in each test case?" << 
      "\n(Number between 1 and 2,147,483,647)" << endl;
      cin >> numbers_per_testcase;
    }
    
    //getting range of each number generated from user
    cout << 
    "\nWhat is the MINIMUM value you would like the randomly generated values to be?"
    << endl;
    cin >> min_value;
    cout << 
    "\nWhat is the MAXIMUM value you would like the randomly generated values to be?"
    << endl;
    cin >> max_value;
  }
}
