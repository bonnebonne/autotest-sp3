/*
 *  File: testsuite.cpp
 *  Authors-Sprint1: Joseph Lillo, Dan Nix, Lisa Woody
 *  Authors-Sprint2: Ryan Brown, Kelsey Bellew, Ryan Feather
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
        //cout << "Could not compile student program: " << program;
        //return false;
    }

    // Crawl child directories for test files.
    if(testFiles.empty())
        dirCrawl(tstExt, ".", testFiles);

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
    return true;
}

// Runs program with input from test files in testFiles vector.
void TestSuite::runTests()
{
    int numCorrect = 0, numWrong = 0;
    int i;
    string name;
    string logName;
    string stored_dir;
    double rate;
    bool crit = false;
    string crit_string = "crit.tst";
    bool crit_passed = true;
    char buff[40];

    //Get directory of current program
    i = testProgram.rfind('.');
    testProgram = testProgram.substr(0, i); 
    // Create directory to store output files.
    stored_dir = "mkdir " + testProgram + "-";
    stored_dir += exeTime;
    system( stored_dir.c_str() );

    // Create log file.
    logName = testProgram;
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
        //Debug
        //cout << *it << endl;
        
        //Determine if this is a critical test
        if(it->find(crit_string) >= 0)
        {
            crit = true;
        }

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
            //If this was a crit test, they auto fail
            if(crit)
            {
                crit_passed = false;
            }
            numWrong++;
            fout << ": FAIL" << endl;
        }
    }

    //If all possible crit tests were passed
    if(crit_passed)
    {
        // Output pass and fail stats.
        rate = ( numCorrect / (double)(numCorrect + numWrong) ) * 100;
        fout << rate <<  "% CORRECT," << numCorrect << " PASSED," << numWrong << " FAILED";
        sprintf(buff, "  %f%% Correct\n", rate); 
        i = testProgram.rfind('/');
        studentResults.push_back(testProgram.substr(i+1) + string(buff)); 
    }
    else
    {
        //If one or more were not passed
        fout << "Failed: Did not pass one or more acceptance tests (Labeled as crit)" << endl;
        i = testProgram.rfind('/');
        studentResults.push_back(testProgram.substr(i) + "  FAILED\n");   
    }
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
    string run_instruction = testProgram + " < ";
    run_instruction += test_file_path;
    run_instruction += " | tee test_out.klein " + testProgram;
    run_instruction += "-" + exeTime;
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
void TestSuite::menu(int& datatype, int& number_of_testcases,
 int& numbers_per_testcase,double& min_value, double& max_value)
{
  
    cout << "--------------------------------------------" << endl;
    cout << "----------Automated Test Generator----------" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "\n\n" << endl;

    //getting data type from user
    cout << "What datatype are the numbers? (1 for ints, 2 for floats)" << endl;
    cin >> datatype;
    while(datatype != (1 || 2))
    {
        cout << "\nIncorrect choice input." << endl;
        cout << 
        "What datatype are the numbers? (1 for ints, 2 for floats)" << endl;
        cin >> datatype;
    }
    
    //getting how many test cases to generate from user
    cout << 
      "\nHow many test cases would you like generated?" << 
      "\n(Number between 1 and 100)" << endl;
    cin >> number_of_testcases;
    while((number_of_testcases < 1) || (number_of_testcases > 100))
    {
        cout << "\nHow many test cases would you like generated?" << 
        "\n(Number between 1 and 100)" << endl;
        cin >> number_of_testcases;
    }
    
    //getting how many numbers to generate per test case from user
    cout << 
      "\nHow many random numbers would you like in each test case?" << 
      "\n(Number between 1 and 200)" << endl;
    cin >> numbers_per_testcase;
    while((numbers_per_testcase < 1) || (numbers_per_testcase > 200))
    {
        cout << 
        "\nHow many random numbers would you like in each test case?" << 
        "\n(Number between 1 and 200)" << endl;
        cin >> numbers_per_testcase;
    }
    
    //getting range of each number generated from user
    cout << 
      "\nWhat is the MINIMUM value you would like the randomly generated values to be?"
      << "\n Number between –2147483648 to 2147483646"<< endl;
    cin >> min_value;
    cout << 
      "\nWhat is the MAXIMUM value you would like the randomly generated values to be?"
      << "\n Number between –2147483647 to 2147483647"<< endl;
    cin >> max_value;
    while(max_value <= min_value)
    {
        cout << "\n Maximum must be largert than mimimum." << endl;
        cout << 
          "\nWhat is the MINIMUM value you would like the randomly generated values to be?"
          << "\n Number between –2147483648 to 2147483646"<< endl;
        cin >> min_value;
        cout << 
          "\nWhat is the MAXIMUM value you would like the randomly generated values to be?"
          << "\n Number between –2147483647 to 2147483647"<< endl;
        cin >> max_value;
	while(max_value <= min_value)
	{
            cout << "\n Maximum must be larger than minimum." << endl;
            cout << 
              "\nWhat is the MINIMUM value you would like the randomly generated values to be?"
              << "\n Number between –2147483648 to 2147483646"<< endl;
            cin >> min_value;
            cout << 
              "\nWhat is the MAXIMUM value you would like the randomly generated values to be?"
              << "\n Number between –2147483647 to 2147483647"<< endl;
            cin >> max_value;
        }	
    }
}
/*Function takes paramaters and generates the specified number of random tests to 
.tst and .ans outfiles.*/
int TestSuite::rand_tests(double max, double min, int type, int num_tests, int num_nums, string goldencpp) //returns 0 for success, -1 for failure
{
    ofstream fout1,fout2;
    double num, range;
    int i, j, spot;
    string s, snum, temp, trueresult;
    FILE *pfile;

    //get range
    range = max - min;

    //get goldencpp without cpp //if goldencpp does in fact come with .cpp
    spot = goldencpp.length();
    for(i = 0; i<goldencpp.length(); i++)
    {
        if(goldencpp[i] == '.' && i != 0)
            spot = i;
    }
    if(spot != goldencpp.length())
        goldencpp = goldencpp.substr(0, (spot));

    //check to see if we were given a non integer

    srand(time(NULL));
    //compiling golden cpp
    string compilecpp = "g++ -o " + goldencpp + " " + goldencpp + ".cpp";
    
    //make tests directory
    //causes a problem if 'tests' already exists
    system("mkdir tests"); 

    system(compilecpp.c_str());
  
    for(j=0; j<num_tests; j++)
    {
        //need to rename these files or they will get overwritten
        //in the case of nultiple tests
        string temp = static_cast<ostringstream*>( &(ostringstream() << num_tests))->str();

        //generate time stamp
        time_t rawTime;
        tm * timeInfo;
        char buffer [40];

        time (&rawTime);
        timeInfo = localtime (&rawTime);

        strftime (buffer,40,"%d_%m_%y_%H_%M",timeInfo);
        string curr_time(buffer);

        string filetst = "tests/generated" + temp + "_" + curr_time + ".tst";
        string fileans = "tests/generated" + temp + "_" + curr_time + ".ans";

        fout1.open(filetst.c_str()); 
        fout2.open(fileans.c_str());
        if(!fout1 || !fout2)
        {
            cout << "An error was occurred creating the output test files.\n";
            return -1;
        }

        //generating random numbers and running them against golden
        for(i=0; i<num_nums; i++)
        {
            //generate an int if numbers were ints, else generate decimal numbers
            if(type == 1)
                num = rand() % int(range)+(max-range+1);
            else
                num = range * ((double)rand()/(double)RAND_MAX) + min;


            //conversion from int to string
            //check that this still works for doubles.  
            snum = static_cast<ostringstream*>( &(ostringstream() << num))->str();
            s = goldencpp + " <<< " + snum;

            pfile = popen(s.c_str(), "r");
            char buff[256];
            while(fgets(buff, sizeof(buff), pfile) != 0)
            {
                string result(buff);
                trueresult = result;
                fout1 << snum << endl;
            }

        }//end num_nums loop
        fout2 << trueresult << endl;
    
        //closing out files
        fout1.close();
        fout2.close();
    }//end num_tests loop

    pclose(pfile);

    return 0;
}

/*Function calls the Menu() function, then finds the golden cpp and calls rand_tests to 
generate the .tst and .ans files for the autogenerated test cases*/
void TestSuite::helper_func()
{
    int datatype, number_of_testcases, numbers_per_testcase;
    double min_value, max_value;
    string goldencpp;
    menu(datatype, number_of_testcases, numbers_per_testcase, min_value, max_value);
    
    //locating the golden cpp
    string dir = ".";
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
        // Watch for files with .cpp extension
        if ( DT_REG == entry->d_type )
        {
            string fileName = entry->d_name;
            size_t extPos = fileName.rfind(".");
            if ( extPos != string::npos )
            {
                string ext = fileName.substr( extPos );
                if ( ".cpp" == ext )
                {
                    fileName = dir + "/" + fileName;
                    cout << fileName << endl;
                    goldencpp = fileName;
                }
            }
        }
    }while((entry=readdir(proc)));
    closedir(proc);
    
    //generates the .tst and .ans files for the randomly generated test cases?
    //pretty sure we need this loop to generate the desired amount of test cases
    //for(int i=0;i<number_of_testcases;i++);
    //{
    int success = rand_tests(max_value, min_value, datatype, number_of_testcases, numbers_per_testcase, goldencpp);
    //}
}

void TestSuite::createSummary()
{
    ofstream fout;
    string outfile = "Summary-";
    outfile += exeTime;
    fout.open(outfile.c_str());

    vector<string>::iterator it;
    for(it = studentResults.begin(); it != studentResults.end(); it++)
    {
        fout << *it;
    }
    
    fout.close();
}
