#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include "git_exec.h"//exec_git_command & exec_git_getsha1
#include "cluster_init.h"//initializes clusters
#include "cluster.h"
#include "history_analysis.h"
#include <cctype>
#include "output.h"

using namespace std;

//reads init versions identificators from file
int Get_Start_SHA1(const char* filename, vector<string>* Start_SHA1, char showflag, ofstream& logfile)
{
    ifstream in_file(filename, ios_base::in);
    if(!in_file.is_open())
    {
        if(showflag == 1)
            logfile << "FILE : " << filename << " COULD NOT BE OPENED. ABORTING..." << std::endl;
        return 1;
    }

    while(!in_file.eof())
    {
        string S_temp;
        getline(in_file, S_temp);
        int it_is_SHA1 = 1;
        for(size_t i = 0; i < S_temp.size() && it_is_SHA1 == 1 && i < 40; ++i)//SHA_1 is 40 symbols long
        {
            if( (isdigit(S_temp[i]) || isalpha(S_temp[i])) == 0 )
                it_is_SHA1 = 0;
        }
        if(it_is_SHA1 == 1 && S_temp.size() == 40)
            Start_SHA1->push_back(S_temp);
    }

    return 0;
}

//this function launches clones finding process
int main_module(int argc, char* argv[], char showflag, ofstream& logfile)
{
    /*ofstream logfile;
    if( showflag == 1 )
    {
        logfile.open( log_file.c_str(), ofstream::out | ofstream::app );
        if( !logfile.is_open() )
        {
            cout << "Failed to open log file:" + log_file + "\nAborting..." << endl;
            return 1;
        }
    }*/

    //PATH_TO_EXE - path to program executable, where also fake_libc_include and python scripts should be located
    string path_to_exe(argv[0]);
    string key("cpr4_cppcheck");
    size_t pos = path_to_exe.rfind( key );
    if( pos != std::string::npos )
        path_to_exe.erase( path_to_exe.begin() + pos, path_to_exe.end() );


    cout << endl << "#GETTING INITIAL DATA..." << endl << endl;
    //string S1("git rev-list --min-parents=0 HEAD");

    //reading init versions identificators (SHA-1) from file in argv[1]
    vector<string> Start_SHA1;

    if(Get_Start_SHA1(argv[1], &Start_SHA1, showflag, logfile) == 1)
    {
        exec_git_command("git checkout master", showflag, logfile);
        return 1;
    }

    if(Start_SHA1.size() > 1)
    {
        Start_SHA1.erase( Start_SHA1.begin() + 1, Start_SHA1.end() );
    }

    //print out SHA1 of all versions in argv[1]
    if( showflag == 1 )
    {
        logfile << "SHA1 hashes of commits used for initializing are:" << endl;
        for(size_t i = 0; i < Start_SHA1.size(); ++i)
            logfile << Start_SHA1[i] << endl;
        logfile << endl;
    }

    //exit if no SHA1 found
    if(Start_SHA1.size() == 0)//should be at least one version in file - one elemtn in vector
    {
        if(showflag == 1)
            logfile << "No hashes in " << argv[1] << "Aborting..." << endl;
        return 1;
    }

    //first element of vector is initial versions from which we'll start
    string S_SHA1(Start_SHA1[0]);
    if( showflag == 1)
        logfile << "Start commit's SHA1 hash is: " << S_SHA1 << endl;

    //vector that contains versions to be processed is created and filled
    vector<Commit_Level> Commit_Levels;
    Commit_Level Level0;
    Level0.level = 0;
    Level0.SHA1_of_commits.push_back(S_SHA1);
    Commit_Levels.push_back(Level0);

    if(Fill_Commit_Levels(&Commit_Levels, &Start_SHA1, showflag, logfile) == 1)
    {
        exec_git_command("git checkout master", showflag, logfile);
        return 1;
    }

    //printing out contents of commit levels
    if( showflag == 1)
    {
        for(size_t i = 0; i < Commit_Levels.size(); i++)
        {
            if( i < 10 )//выравнивание т.к. появляются двузначные числа
                logfile << "COMMIT LEVEL :  " << Commit_Levels[i].level << " : ";
            else
                logfile << "COMMIT LEVEL : " << Commit_Levels[i].level << " : ";

            for(size_t j = 0; j < Commit_Levels[i].SHA1_of_commits.size(); ++j)
                logfile << Commit_Levels[i].SHA1_of_commits[j] << "   ";

            logfile << endl;
        }
        logfile << endl;
    }


    cout << endl << "#APPLYING CPPCHECK..." << endl << endl;

    if(Analyze_History(&Commit_Levels, path_to_exe, showflag, logfile) == 1)
    {
        exec_git_command("git checkout master", showflag, logfile);
        return 1;
    }

    cout << endl << "#PROCESS IS OVER." << endl << endl;

    return 0;
}

int main(int argc, char* argv[])
{
    //181ed5489bfc64cc0f241f385f1d24f3241cb155 - example of SHA1 - used just for debugging
    if(argc == 2)//there must be strictly 1 command line argument
    {
        char showflag = 0;
        ofstream filler_logfile;

        if( main_module(argc, argv, showflag, filler_logfile) != 0 )
        {
            exec_git_command("git checkout master", 0, filler_logfile);
            return 1;
        }

    }
    else
    {
        if( argc == 4 )//adding -show and '../.../file' - for output
        {
            string argv4(argv[2]);
            if( argv4 == "-show" )
            {
                char showflag = 1;
                string log_file(argv[3]);

                ofstream logfile;
                logfile.open( log_file.c_str(), ofstream::out | ofstream::trunc );

                if( !logfile.is_open() )
                {
                    cout << "Failed to open log file:" + log_file + "\nAborting..." << endl;
                    return 1;
                }


                if( main_module(argc, argv, showflag, logfile) != 0 )
                {
                    logfile.close();
                    if( logfile.is_open() )
                        logfile.close();

                    exec_git_command("git checkout master", 0, logfile);
                    return 1;
                }

                logfile.close();
                    if( logfile.is_open() )
                        logfile.close();
            }
        }
        else
        {
            cout << "Exactly three parameters are needed - file with SHA1 hashes of starting commits, fragment size and path to output file that describes cluster without extension." << endl;
            cout << "For example: ../Course_Realization/bin/Debug/Course_Realization '../sha1.txt' 3 '../output'" << endl;;
        }
    }

    cout << endl;
    ofstream filler_logfile;
    //returning to initial repository state - when this program was not applied
    exec_git_command("git checkout master", 0, filler_logfile);

    return 0;
}
