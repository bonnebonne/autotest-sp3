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

#include <unistd.h>
TestSuite::TestSuite()
{
    profiling = false;
}
//Function to compile c++ source code based on filename
bool TestSuite::compile_code( string filename ) {

    int i = filename.rfind('.');
    string compile_instruction = "g++ ";
    compile_instruction += filename;
    compile_instruction += " -o ";

    compile_instruction += filename.substr(0, i);


    if(!system( compile_instruction.c_str() ))
    {
        return false;
    }

    return true;
}

//Function to compile student code with gcov and gprof flags
bool TestSuite::compile_student_code( string filename )
{
    char path[512] = "";
    getcwd(path, sizeof(path));
	string gcov_profile_cmd;
	// compile with profiling
	if(profiling)
		gcov_profile_cmd ="g++ -Wall -pg -g -fprofile-arcs -ftest-coverage ";
	else
		gcov_profile_cmd ="g++ -Wall -g -fprofile-arcs -ftest-coverage ";

    int j = filename.rfind('/') + 1;
    string output = filename.substr(j, filename.length() - 1);
    output = output.substr(0, output.rfind('.'));

    gcov_profile_cmd += " -o ";
    gcov_profile_cmd += output + " ";
    gcov_profile_cmd += filename.substr(j, filename.length() - 1);
    string chng_dir(filename.substr(0, j));
	
	// change into students directory to compile code
    chdir(chng_dir.c_str());
    system(gcov_profile_cmd.c_str());
    chdir(path);
	
    return true;
}

// This function retrieves gcov information and appends it to the student
// log file
// NOTE: gcov deposits code coverage files in the directory it is run from.
// Therefore, instead of calling gcov <directory_path>/<source_code>,
// <source_code> only needs to passed to the gcov command
string TestSuite::get_gcov( string filename )
{
    char path[512] = "";
    getcwd(path, sizeof(path));

    // run gcov
    /*#ifdef __cpluspluss
           __gcov_flush();
    #endif*/
    string run_gcov("gcov ");
    int i = filename.rfind('/') + 1;
    run_gcov += filename.substr(i, filename.length() - 1);
    run_gcov += " > ";
    run_gcov += "summary.gcov";

	if ( !infinite_loop )
	{
    	// change into student source code directory
    	chdir((filename.substr(0, i)).c_str());

    	system(run_gcov.c_str());
    	// get gcov info for student log files
    	ifstream fin;
    	fin.open( "summary.gcov" );
    	if(!fin)
    	{
        	cout << "summary.gcov failed to open" << endl;
        	chdir(path); // change to class (parent) directory
        	return "";
    	}
    	string line;
    	char c_line[512] = "";
    	fin.getline(c_line, 512, '\n'); // ignore first line
    	fin.getline(c_line, 512, '\n'); // this is the line we want, it has the code coverage
    	line = c_line;
		// Clean up gcov files
		system("rm -f *.gcov");
		system("rm -f *.gcno");
		system("rm -f *.gcda");

	
    	chdir(path); // change to class (parent) directory    
		return line;
	}
	return "infLoop";
}

// This function retrieves the gprof profiling information
// of functions that take more than 0.0 run time
string TestSuite::get_gprof( string filename, ofstream &fout )
{
    char path[512] = "";
    getcwd(path, sizeof(path));
    string run_gprof("gprof -p -b "), temp = "";
	char line[256] = "";
	ifstream fin;
	float time = 0.0;
    int i = filename.rfind('/') + 1;
    run_gprof += filename.substr(i, filename.length());
    run_gprof += " gmon.out > profile.out";

	if ( !infinite_loop)
	{
    	// change into student source code directory
    	chdir((filename.substr(0, i)).c_str());
    	system(run_gprof.c_str());
		fin.open("profile.out");
		if(!fin)
			return "gprof retrieval failed";
		while(fin >> temp) 
		{
			if(temp == "time")
				break;
			fin.ignore(256, '\n');
		}
		while(fin >> time)
		{
			if(time > 0.0)
			{
				fin.getline(line, 256, '\n');
				fout << time << " " << line << endl;
			}
		}
		fin.close();
		
		// Clean up gprof files
		system("rm -f gmon*");
		system("rm -f *.gcda");
		system("rm -f profile.out");
	
    	chdir(path); // change back to parent directory
		
	}
    return "";
}
// Initialize the testing session.
bool TestSuite::initTest(string program, string tstExt, string ansExt)
{
    testProgram = program;
    testExtension = tstExt;
    answerExtension = ansExt;
	string dummy;	


    // Compile Test Programs
    compile_student_code(program);

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

    // Create log file.
    logName = testProgram + "-";
    logName += exeTime;
    logName += ".log";
	ifstream profile;
    ofstream fout(logName.c_str());
    if (!fout)
    {
        return;
    }

	infinite_loop = false;
    // Iterate over test files.
    vector<string>::iterator it;
    for ( it = testFiles.begin(); it != testFiles.end() ; it++ )
    {
        //Determine if this is a critical test
        if(it->find(crit_string) != string::npos)
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
        if ( !infinite_loop )
            run_code(*it,name);

		//else, do a failed program log file i supposd

        // Determine corresponding answer file.
        string ans = *it;
        ans.replace(ans.end()-4, ans.end(),answerExtension);

        if (!infinite_loop)
        {
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
        else
        {
            fout << " :FAIL Infinite Loop" << endl;
			break;
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


    fout << "\n" <<  get_gcov(testProgram) << endl;
	
    if(profiling)
        fout << get_gprof(testProgram, fout) << endl;
		
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
                dirCrawl( targetExt, newDir, dest);
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
                    dest.push_back(fileName);
                }
            }
        }
    } while((entry=readdir(proc)));

    closedir(proc);
}

//Function to run c++ souce with redirected input/output
//  This function will fork to produce a child process then wait for its 
// completion, this process will be the student source code running. If the 
// process takes more than the allowable amount of time it is killed. This 
// function use microseconds as to not slow donw programs without inf_loops 
int TestSuite::run_code( string test_file_path, string test_file_name ){
//bool TestSuite::run_code( string test_file_path, string test_file_name ) {

    //This instruction will run the test program with test_file_path piped in.
    //The output will be piped to test_out.klein and also a file in the
    //timestamped output file directory. The klein file is used for comparing
    //the output to the expected value.
	int wait_pid, childpid;
	int time_limit = allowed_time * 10000;
	bool inf_loop = false;
	int fpt1, fpt2;	
	int status;
	int timer = 0;

    char path[512] = "";
    getcwd(path, sizeof(path));
    string dir_path = path;
    int i = testProgram.rfind('/');
    string run_instruction = "./" + testProgram.substr(i + 1, testProgram.length())
                             + " < ";
    run_instruction += dir_path + test_file_path.substr(1, test_file_path.length());
    run_instruction += " > " + dir_path + "/test_out.klein";
	childpid = fork();


	if (childpid == 0)
	{
    	fpt1 = open(test_file_path.c_str(), O_RDONLY);
		fpt2 = creat("dummy.out", 0644);
	
		close(0);
		dup(fpt1);
		close(fpt1);

		close(1);
		dup(fpt2);
		close(fpt2);

		execvp(testProgram.c_str() ,NULL);
	}   
    while (true)
	    {
	    // sleep one second and see if process is done
        usleep (100);
        timer ++;
        wait_pid = waitpid(childpid, &status, WNOHANG);

        // if process is done, break out of loop
        if (wait_pid != 0)
          	break;

		if (timer >= time_limit)
	    	{
				//insert failed code because of infinite loop
                inf_loop = true;				
                kill(childpid, 9);
            }
		
		}
	

	if (inf_loop)
    {	//do stuff for failing because of inifinite loop
        infinite_loop = true;
		return 0;
    }
    else
    {
        chdir((testProgram.substr(0, i )).c_str());
        system( run_instruction.c_str() );
        chdir(path);
    }

    return 1;
}


//Function to do diff on answer file and test program output file
bool TestSuite::correct_answer( string ans_file )
{
	// NOTE vars are initiated where and when they are needed
	// to improve efficiency (this function gets called ALOT)
    if(presentationErrors)
    {
		
        bool pass = false;
		ifstream ans, sol;
		ans.open(ans_file.c_str());
		sol.open("test_out.klein");
		if(!ans || !sol)
		{
			cout << "Answer of solution failed to open." << endl;
			return true; // record test as passed even though
			// test didn't work
		}
		
        if(stringPresentationErrors)
		{
			string ans_str = "", sol_str = "", temp = "";
            while(sol >> temp)
            {

                sol_str += temp;
                cout << "solstr: " << temp << endl;
                temp = "";
            }
			while(ans >> temp)
            {
				ans_str += temp;
                temp = "";
            }

            pass = closeEnoughString(sol_str, ans_str);
        }
        else
        {
			float ans_flt = 0.0, sol_flt = 0.0;
			ans >> ans_flt;
            sol >> sol_flt;
            cout << "sol_flt: " << sol_flt << endl;
            pass = closeEnoughFloat(sol_flt, ans_flt);
        }
		
		ans.close();
		sol.close();
		if(pass) cout << "passed!" << endl;
        return pass;
    }
    
    string diff_instruction = "diff test_out.klein ";
    diff_instruction += ans_file;
    diff_instruction += " &> /dev/null";

    return (! system( diff_instruction.c_str() ) );

}

// finds student cpps
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

	cout << entry->d_name << endl;
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

    } while((entry=readdir(proc)));

    closedir(proc);

    return;
}
 // this function will generate the test files used to test menus if there 
 // is a .spec file in the root directory. This function asks for max, min and 
 // number of tests cases. The .spec file is read in repeatedly (once for each
 // generated test case) to produce the .tst files. After the cases are generated
 // the .ans files are generated using old code from sprint 2
bool TestSuite::menu_tests( string spec_file_path )
{
	ifstream fin;
	ofstream fout_tst, fout_ans;
	
	//read var, and .tst and .ans filenames
    string tst_filename = "tests/menu_test_"; 
    string ans_filename = "tests/menu_test_";
	string read;
	stringstream ss;  //itoa essentially

	int num_test_files = -1;
	double max, min = 0;
	int rand_int = 0;
	float rand_float = 0;

	srand (time(NULL));	
	system("mkdir -p tests");

	char ans[128] = {'\0'};
    locateGolden();
	//ask if want to generate tests for menus
	while (strcmp(ans,"y") && strcmp(ans,"n") )
	{
		for (int i = 0; i < 128; i++)
			ans[i] = '\0'; 
        cout << "Would you like to generate files for testing menus? (y/n): " << endl;
		cin >> ans;
	}

	if (!strcmp(ans,"y") )
	{
		// open .spec file check for success
		fin.open(spec_file_path.c_str());
        if (!fin)
        {
            cout << "spec file failed to open." << endl;
            return true;
       }

            fin.close();
		// if so get input values
		//display menu for range, number of test cases
        while ( num_test_files <= 0 || num_test_files > 100)
		{
			cout << "Enter number of test files to generate for menu testing (between 0 and 100): ";
			cin >> num_test_files;
			cout << endl;
		}
	
		cout <<
    		"\nWhat is the MINIMUM value you would like the randomly generated values to be?"
    	     << "\n Number between –2147483648 to 2147483646"<< endl;
    	cin >> min;
    	cout <<
    	     "\nWhat is the MAXIMUM value you would like the randomly generated values to be?"
    	     << "\n Number between –2147483647 to 2147483647"<< endl;
    	cin >> max;
    	while(max <= min)
    	{
    	    cout << "\n Maximum must be larger than mimimum." << endl;
    	    cout <<
    	         "\nWhat is the MINIMUM value you would like the randomly generated values to be?"
    	         << "\n Number between –2147483648 to 2147483646"<< endl;
    	    cin >> min;
    	    cout <<
    	         "\nWhat is the MAXIMUM value you would like the randomly generated values to be?"
    	         << "\n Number between –2147483647 to 2147483647"<< endl;
    	    cin >> max;
    	}
	
		//generate the floats and ints
		double range = max - min;
		//generate .tst files
		for (int i = 0; i < num_test_files;i++)
		{
			string test_filename = tst_filename;
			string answer_filename = ans_filename;
			//generate time stamp
    	    time_t rawTime;
    	    tm * timeInfo;
    	    char buffer [40];
			string i_to_string = ""; //create the filenames
			ss.str("");
	
    	    time (&rawTime);
    	    timeInfo = localtime (&rawTime);
	
    	    strftime (buffer,40,"%d_%m_%y_%H_%M_%S",timeInfo);
    	    string curr_time(buffer);
			
			ss << i+1;
			i_to_string = ss.str();
	
			test_filename += i_to_string;
			answer_filename += i_to_string;
			test_filename = test_filename + "_"	+ curr_time + ".tst";
			answer_filename = answer_filename + "_"	+ curr_time + ".ans";
			//most code above just creates filenames				

			//open spec file repeatedly to get values in files
			fin.open(spec_file_path.c_str());
            //fout_ans.open(answer_filename.c_str());
			fout_tst.open(test_filename.c_str());
			//open .tst and .ans files for output
			//generate the tests files
			while ( fin >> read )
			{	
				//atoi returns 0 if it is a string value, use in if stmts
				if (atoi(read.c_str()))
					fout_tst << read << endl;
                else if ( !atoi(read.c_str()) )
				{
					if (read == "int")
					{
					 	rand_int = rand() % int(range)+(max-range+1);
						fout_tst << rand_int << endl;	
					}            	
					else
					{
    	            	rand_float = range * ((double)rand()/(double)RAND_MAX) + min;
						fout_tst << rand_float << endl;
					}		
				}		
								
			}
			fout_tst.close();
			fin.close();
			
			//create .ans files 
            string s = goldencppGlobal.substr(0, goldencppGlobal.rfind('.')) + " < " + test_filename + " > " + answer_filename;
            cout << s << endl;
    	    FILE *pfile = popen(s.c_str(), "r");
    	    char buff[256];
    	    while(fgets(buff, sizeof(buff), pfile) != 0)
    	    {
    	        string result(buff);

    	        fout_ans << result;
    	    }
	
    	    //closing out files
    	    //fout1.close();
    	    fout_ans.close();
    		pclose(pfile);

			
		}
        return true;
	}
	else 
        return false;

}

// Function gathers the required data from the user and returns all of the
// values by reference.
void TestSuite::menu(int& datatype, int& number_of_testcases,
                     int& numbers_per_testcase,double& min_value, double& max_value,
					 int& string_length, bool& exact_length)
{

	char temp_exact;

    cout << "--------------------------------------------" << endl;
    cout << "----------Automated Test Generator----------" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "\n\n" << endl;

    //getting data type from user
    cout << "What datatype are the values? (1 for ints, 2 for floats, 3 for strings)" << endl;
    cin >> datatype;
    while(datatype != 1 && datatype != 2 && datatype != 3)
    {
        cout << "\nIncorrect choice input." << endl;
        cout <<
             "What datatype are the numbers? (1 for ints, 2 for floats, 3 for strings)" << endl;
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
         "\nHow many random values would you like in each test case?" <<
         "\n(Number between 1 and 200)" << endl;
    cin >> numbers_per_testcase;
    while((numbers_per_testcase < 1) || (numbers_per_testcase > 200))
    {
        cout <<
             "\nHow many random values would you like in each test case?" <<
             "\n(Number between 1 and 200)" << endl;
        cin >> numbers_per_testcase;
    }
	if (datatype == 3)
	{
		cout <<
			 "\nHow long do you want the generated strings to be?" <<
			 "\n(Number between 1 and 80)" << endl;
		cin >> string_length;
		while((string_length < 1) || (string_length > 100))
		{
			cout << "\nHow many test cases would you like generated?" <<
				 "\n(Number between 1 and 80)" << endl;
			cin >> string_length;
		}
	    cout <<
         "\nDo you want possible strings to include ones shorter than this length?" <<
         "\n(y or n)" << endl;
		cin >> temp_exact;
		while(temp_exact != 'y' && temp_exact != 'n')
		{
			cout <<          
				"\nDo you want possible strings to include ones shorter than this length?" <<
				"\n(y or n)"<< endl;
			cin >> temp_exact;
		}
		if( temp_exact == 'y')
			exact_length = true;
		else
			exact_length = false;

	}

	else
	{
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
}
//Function takes paramaters and generates the specified number of random tests to
//.tst and .ans outfiles.
int TestSuite::rand_tests(double max, double min, int type, int num_tests, int num_nums, int string_length, bool exact_length, string goldencpp) //returns 0 for success, -1 for failure
{
    ofstream fout1,fout2;
    double num, range;
    int i, j, spot;
    string s, snum, temp, trueresult, str = "";
	char tempChar[2] = {'\0'};
    FILE *pfile;

    srand(time(NULL));
    //get range
    range = max - min;

    //get goldencpp without cpp //if goldencpp does in fact come with .cpp
    spot = goldencpp.length();
    for(i = 0; i<(int)goldencpp.length(); i++)
    {
        if(goldencpp[i] == '.' && i != 0)
            spot = i;
    }
    if(spot != (int)goldencpp.length())
        goldencpp = goldencpp.substr(0, (spot));

    //check to see if we were given a non integer

    //compiling golden cpp
    string compilecpp = "g++ -o " + goldencpp + " " + goldencpp + ".cpp";

    //make tests directory
    //causes a problem if 'tests' already exists
    system("mkdir -p tests");

    system(compilecpp.c_str());

    for(j=0; j<num_tests; j++)
    {
        //need to rename these files or they will get overwritten
        //in the case of nultiple tests
        string temp = static_cast<ostringstream*>( &(ostringstream() << (j+1)))->str();

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
        //fout2.open(fileans.c_str());
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
            else if(type == 2)
                num = range * ((double)rand()/(double)RAND_MAX) + min;
			else
			{
				if(exact_length == true)
				{
					for(int i = 0; i < string_length; i++)
					{
						tempChar[0] = rand()%26+97;
						str.append( tempChar );
					}
				}
				else
				{
					int length = rand()%80 + 1;
					for(int i = 0; i < length ; i++)
					{
						tempChar[0] = rand()%26+97;
						str.append( tempChar );
					}
				}
                fout1 << str <<endl;
                str = "";
			}

            //conversion from int to string
            //check that this still works for doubles.
            if(type != 3)
			{
				snum = static_cast<ostringstream*>( &(ostringstream() << num))->str();
				fout1 << snum << endl;
			}
            //}

        }//end num_nums loop

        fout1.close();
        //fin.open(filetst.c_str());
        //fout2 << trueresult << endl;

        s = goldencpp + " < " + filetst + " > " + fileans;
        system(s.c_str());

    }//end num_tests loop

    return 0;
}

// Function calls the Menu() function, then finds the golden cpp and calls rand_tests to
//generate the .tst and .ans files for the autogenerated test cases
void TestSuite::helper_func()
{
    int datatype, number_of_testcases, numbers_per_testcase, string_length;
    double min_value, max_value;
	bool exact_length;
    string goldencpp;
    menu(datatype, number_of_testcases, numbers_per_testcase, min_value, max_value, string_length, exact_length);
    locateGolden();
    goldencpp = goldencppGlobal;
    //generates the .tst and .ans files for the randomly generated test cases?
    //pretty sure we need this loop to generate the desired amount of test cases
    //int success =
    rand_tests(max_value, min_value, datatype, number_of_testcases, numbers_per_testcase, string_length, exact_length, goldencpp);
}

// locates golden cpp and sets it as 
void TestSuite::locateGolden()
{
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
                    goldencppGlobal = fileName;
                }
            }
        }
    } while((entry=readdir(proc)));
    closedir(proc);
}

// Function creates the overall class summary
void TestSuite::createSummary()
{
    ofstream fout;
    string outfile = "Summary-";
    outfile += exeTime;
	outfile += ".log";
    fout.open(outfile.c_str());

    vector<string>::iterator it;
    for(it = studentResults.begin(); it != studentResults.end(); it++)
    {
        fout << *it;
    }

    fout.close();
}


// Determines whether or not a string is close enough to correct.
// Returns true if it is, false if it isn't.
bool TestSuite::closeEnoughString(string str1, string str2)
{
	int str1_count[26] = {0};
	int str2_count[26] = {0};
	
	//set all characters to lower case
	for(int i = 0; i< (int) str1.length(); i++)
	{
        str1[i] = tolower(str1[i]);
	}
	//set all characters to lower case
	for(int i = 0; i< (int)str2.length(); i++)
	{
		str2[i] = tolower(str2[i]);
	}
	
	//remove all spaces from str1
    for(int i = 0; i < (int)str1.length(); i++)
	{
		if(str1[i] == ' ')
		{
			str1.replace(i, 1, "");
			i--;
		}
	}

	//remove all spaces from str2
    for(int i = 0; i < (int)str2.length(); i++)
	{
		if(str2[i] == ' ')
		{
			str2.replace(i, 1, "");
			i--;
		}
	}

	//if the lengths are the change
	if(str1.length() == str2.length())
	{
		//increment the "used" arrays for each character in each string
        for(int i = 0; i < (int)str1.length(); i++)
		{
			str1_count[str1[i]-97]++;
			str2_count[str2[i]-97]++;
		}
		//return false as soon as two values in the arrays aren't the same
		for(int i = 0; i < 26; i++)
		{
            if(str1_count[i] != str2_count[i])
			{
				return false;
			}
		}
		return true;
	}
	//if the first and last values are the same
	else if(str1[0] == str2[0] && str1[str1.length() - 1] == str2[str2.length() - 1])
	{
		return true;
	}
	
	//if nothing else has happened, return false
	return false;
}


//If the answer provided rounds to the correct answer, mark it as correct
// Note that this function only works one way, it doesn't try to round the
// correct answer to match the given answer.
bool TestSuite::closeEnoughFloat(float provided, float answer)
{
	cout << provided << " =? " << answer << endl;
	while(provided != (int)provided && answer != (int)answer)
	{
		provided = provided * 10;
		answer = answer * 10;
	}

	int rounded = (int)provided;

	if(provided - rounded >= 0.5)
	{
		rounded++;
	}

	if(rounded == answer)
		return true;

	return false;
}

// This function runs presentation menu
void TestSuite::presentationMenu()
{
    string presentationOpt = "", presentationType = "";
    bool validOpt = false;
        while(presentationOpt != "y" && presentationOpt != "n" &&
			  presentationOpt != "Y" && presentationOpt != "N")
        {
            cout << "Do you want to ignore presentation errors? (y/n): ";
            cin >> presentationOpt;
			cout << endl;
        }

        if(presentationOpt == "y")
            presentationErrors = true;
        else
            presentationErrors = false;

    if(presentationOpt == "y")
    {
        validOpt = false;
        while(!validOpt)
        {
            cout << "What datatype presentation errors do you want to ignore?";
            cout << " (1 for floats, 2 for string): ";
            cin >> presentationType;

            if(presentationType == "1")
            {
                stringPresentationErrors = false;
                break;
            }
            else if(presentationType == "2")
            {
                stringPresentationErrors = true;
                break;
            }
        }

        presentationErrors = true;
    }
    else
        presentationErrors = false;
}
