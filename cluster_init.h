#ifndef CLUSTER_INIT_H_INCLUDED
#define CLUSTER_INIT_H_INCLUDED

#include "cluster.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include <cctype>
#include <cmath>
#include "git_exec.h"
#include "lexical.h"

using namespace boost::filesystem;
struct recursive_directory_range
{
    typedef recursive_directory_iterator iterator;
    recursive_directory_range(path p) : p_(p) {}

    iterator begin() { return recursive_directory_iterator(p_); }
    iterator end() { return recursive_directory_iterator(); }

    path p_;
};

int string_found_C_extension(std::string path)
{
    bool Found = (path.find(".c", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".H", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".hh", path.size() - 3) != std::string::npos);
   // Found = Found || (path.find(".hpp", path.size() - 4) != std::string::npos);
    //Found = Found || (path.find(".h++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".hxx", path.size() - 4) != std::string::npos);

    Found = Found || (path.find(".h", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".C", path.size() - 2) != std::string::npos);
    //Found = Found || (path.find(".cc", path.size() - 3) != std::string::npos);
    //Found = Found || (path.find(".cpp", path.size() - 4) != std::string::npos);
    //Found = Found || (path.find(".c++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".cxx", path.size() - 4) != std::string::npos);

   // Found = Found || (path.find(".i", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".I", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".ii", path.size() - 3) != std::string::npos);
   // Found = Found || (path.find(".ipp", path.size() - 4) != std::string::npos);
    //Found = Found || (path.find(".i++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".inl", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".ixx", path.size() - 4) != std::string::npos);

   // Found = Found || (path.find(".t", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".T", path.size() - 2) != std::string::npos);
   // Found = Found || (path.find(".tt", path.size() - 3) != std::string::npos);
   // Found = Found || (path.find(".tpp", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".t++", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".tpl", path.size() - 4) != std::string::npos);
   // Found = Found || (path.find(".txx", path.size() - 4) != std::string::npos);

    return Found;
}

int list_dir_contents(std::vector<std::string>* Paths, char showflag, ofstream& logfile)
{
    if(showflag == 1)
        logfile << "LISTING FILES WITH ACCEPTABLE EXTENSIONS : " << std::endl;
    for (directory_entry it : recursive_directory_range("."))
    {
        std::string path = it.path().string();
        if(path.find(".git") == std::string::npos && (string_found_C_extension(path)))
        {
            if(showflag == 1)
                logfile << path << std::endl;
            Paths->push_back(path);
        }
    }

    if(showflag == 1)
        logfile << std::endl;
  /*std::copy(
  boost::filesystem::recursive_directory_iterator("./"),
  boost::filesystem::recursive_directory_iterator(),
  std::ostream_iterator<boost::filesystem::directory_entry>(std::cout, "\n"));*/
return 0;
}

int Is_String_Not_Empty(std::string S_temp)//1 - does contain non-space characters, 0 otherwise
{
    int String_Not_Empty = 0;
    for(size_t t = 0; t < S_temp.size() && String_Not_Empty != 1; ++t)
    {
        char c = S_temp[t];
        if(isspace(c) == 0 && c != EOF)
            String_Not_Empty = 1;
    }
    return String_Not_Empty;
}

void Delete_Extra_Spaces(std::string* Str)//deletes space symbols except \n and comment
{
    int beginning_flag = 1, whitespace_row = 0;//beginning_flag means that we are at the beginning of the string
    //it is needed for erasing all the whitespaces in the beginning, before the first !isspace() symbol
    //whitespace_row is a flag, that contains information whether we encountered a space and all the next space symbols may be erased
    //until we find !isspace() symbol
    size_t i = 0;//position in the string. it is increased by 1 only if Str[i] wasn't erased
    while(i < Str->size())
    {
        if(beginning_flag == 1)
        {
            if(isspace((*Str)[i]) && (*Str)[i] != '\n')
            {
                Str->erase(Str->begin());
                i = 0;//as we erase all the spaces at the beginning of the string we remain at position Str[0]
            }
            else
            {
                beginning_flag = 0;
                i++;
            }
        }
        else
        {
            if(isspace((*Str)[i]) && (*Str)[i] != '\n')
            {
                if(whitespace_row == 1)
                {
                    Str->erase(Str->begin() + i);
                }
                else//it's first whitespace in a row
                {
                    (*Str)[i] = ' ';
                    whitespace_row = 1;//if whitespace row was not == 1, then we only make its value equal 1, as we don't delete the first space in the row
                    i++;
                }
            }
            else
            {
                whitespace_row = 0;//if encountered symbol isn't space symbol, whitespace_row is set 0
                i++;
            }
        }
    }

    if( isspace((*Str)[Str->size() - 1]) && (*Str)[Str->size() - 1] != '\n')//deleting last space symbol
        Str->erase(Str->begin() + Str->size() - 1);

    for( size_t i = Str->size() - 1; i >= 0 && isspace( (*Str)[i] ); --i)
        Str->erase( Str->begin() + i );

//serious doubt! deleting commentaries at end of line
    /*if( Str->rfind(";//") != string::npos )
        Str->erase( Str->rfind(";//") + 1,  Str->size() - Str->rfind(";//") - 1);

    if( Str->rfind("; //") != string::npos )
        Str->erase( Str->rfind("; //") + 1,  Str->size() - Str->rfind("; //") - 1);

    if( Str->rfind(")//") != string::npos )
        Str->erase( Str->rfind(")//") + 1,  Str->size() - Str->rfind(")//") - 1);

    if( Str->rfind(") //") != string::npos )
        Str->erase( Str->rfind(") //") + 1,  Str->size() - Str->rfind(") //") - 1);

    if( Str->rfind("{//") != string::npos )
        Str->erase( Str->rfind("{//") + 1,  Str->size() - Str->rfind("{//") - 1);

    if( Str->rfind("{ //") != string::npos )
        Str->erase( Str->rfind("{ //") + 1,  Str->size() - Str->rfind("{ //") - 1);

    if( Str->rfind("}//") != string::npos )
        Str->erase( Str->rfind("}//") + 1,  Str->size() - Str->rfind("}//") - 1);

    if( Str->rfind("} //") != string::npos )
        Str->erase( Str->rfind("} //") + 1,  Str->size() - Str->rfind("} //") - 1);

    if( Str->size() >= 2 && (*Str)[0] != '/')
        if( Str->find("//") != string::npos )
            Str->erase( Str->rfind("//") + 1,  Str->size() - Str->rfind("//") - 1 );*/

    //deleting figure braces at the end and beginning of the string - they can be located at different lines, but fragment still can be clones
    /*if((*Str)[Str->size() - 1] == '}')
        Str->erase(Str->begin() + Str->size() - 1);

    if( Str->size() >= 2 && (*Str)[Str->size() - 2] == ')' && (*Str)[Str->size() - 1] == '{' )
        Str->erase(Str->begin() + Str->size() - 1);

    if( Str->size() >= 1 && (*Str)[0] == '{' )
        Str->erase(Str->begin() + Str->size() - 1);*/

    return;
}

int find_locations(vector<size_t>* locations, string source, string str_to_find, size_t step)
{
    if(source.find(str_to_find) != string::npos)
    {
        locations->push_back(source.find(str_to_find));
        while((*locations)[locations->size() - 1] + step < source.size())
        {
            size_t i = (*locations)[locations->size() - 1] + step;
            if(source.find(str_to_find, i) == string::npos)
            {
                break;
            }
            else
            {
                locations->push_back(source.find(str_to_find, i));
            }
        }
    }

    return 0;
}

int gen_id(vector<string> Text, vector<string>* Text_out, vector<vector<string>>* IDs, vector<vector<size_t>>* ID_locations, char showflag, ofstream& logfile)
{
    for(size_t i = 0; i < Text.size(); ++i)
    {
        string temp_str_out;
        vector<string> i_a_o;
        if( Parametrization(Text[i], &temp_str_out, &i_a_o, showflag, logfile ) == 1 )
        {
            if(showflag == 1)
                logfile << "Was unable to parametrize " << Text[i] << endl;
            return 1;
        }
        Text_out->push_back(temp_str_out);
        IDs->push_back(i_a_o);

        vector<size_t> locations_id_for_line;
        find_locations(&locations_id_for_line, temp_str_out, "ID", 2);
        ID_locations->push_back(locations_id_for_line);
    }

    return 0;
}

vector<string> gen_typedefs(vector<vector<size_t>> ID_locations, vector<vector<string>> IDs)
{
    vector<string> typedefs;

    if(ID_locations.size() != IDs.size())
    {
        typedefs.push_back("FAIL");
        return typedefs;
    }

    for(size_t i = 0; i < ID_locations.size(); ++i)
    {
        vector<size_t> line_id_locks = ID_locations[i];
        for(size_t j = 0; j < line_id_locks.size(); ++j)
        {
            if(line_id_locks[j] == line_id_locks[j - 1] + 2)
                if( typedefs.end() == find( typedefs.begin(), typedefs.end(), (IDs[i])[j - 1] ) )
                    typedefs.push_back((IDs[i])[j - 1]);
        }
    }

    return typedefs;
}

int braces_balance(vector<string>* v_Str)
{
    int l_brace_counter = 0, r_brace_counter = 0;
    size_t r_brace_beg_counter = 0;

    for(size_t i = 0; i < v_Str->size(); ++i)//each line
    {
        string line = (*v_Str)[i];

        size_t j = line.size() - 1;

        char endflag = 0;
        while(j >= 0 && endflag != 1)
        {
            if( line[j] == '}' )
            {
                if (l_brace_counter == 0)
                {
                    ++r_brace_beg_counter;
                }
                else
                {
                    ++r_brace_counter;
                }
                --j;
            }
            else
            {
                if( j >= 1 && line[j] == ';' && line[j - 1] == '}' )
                {
                    if (l_brace_counter == 0)
                    {
                        ++r_brace_beg_counter;
                    }
                    else
                    {
                        ++r_brace_counter;
                    }
                    j -= 2;
                }
                else
                {
                    endflag = 1;
                }
            }
        }

        /*if(line[line.size() - 1] == '{')
        {
            ++l_brace_counter;
        }*/
        j = 0;
        while( j < line.size() && line[j] == '{' )
        {
            ++l_brace_counter;
            ++j;
        }
    }

    string add_r_brace_beg;
    for(size_t q = 0; q < r_brace_beg_counter; ++q)
    {
        add_r_brace_beg += "{";
    }
    v_Str->insert(v_Str->begin(), add_r_brace_beg);


    if(l_brace_counter > r_brace_counter)
    {
        string addition;
        for(int i = 0; i < l_brace_counter - r_brace_counter; ++i)
            addition += "};";
        v_Str->push_back(addition);
    }
    else
    {
        if(r_brace_counter > l_brace_counter)
        {
            string addition;
            for(int i = 0; i < r_brace_counter - l_brace_counter; ++i)
                addition += "{";
            v_Str->insert(v_Str->begin(), addition);
        }
        else
        {

        }
    }


    return 0;
}

int write_to_file(string filename, vector<string> text)
{
    ofstream outfile;
    outfile.open( filename.c_str() , ofstream::out | ofstream::trunc);

    for(size_t i = 0; i < text.size(); ++i)
    {
        outfile << text[i] << endl;
    }

    outfile.close();

    if(outfile.is_open())
        outfile.close();

    return 0;

}

//this function is needed because when initializing compared can be a part of original
int Exemplars_Are_Equal(Exemplar Original, Exemplar Compared, string path_to_fake_libc, char showflag, ofstream& logfile)// returns 1 if clones, 0 if not
{
    int offset  = ((int)Original.fragment.size() - (int)Compared.fragment.size()) / 2;
    Exemplar small = Compared, big = Original;

    if(offset < 0)//if original is included in compared
    {
        small = Original;
        big = Compared;
    }

    //making fragments have same size
    offset = abs(offset);
    size_t i = 0;
    std::vector<string> first, second;
    for(; i < small.fragment.size(); ++i)
    {
        first.push_back( small.fragment[i] );
        second.push_back( big.fragment[offset + i] );
    }

    //generating versions of first and second where identificators have been changed for ID in text_out_i, stored in ID_i and their locations stored in ID_locs_i
    std::vector<std::string> Text_out_1;
    std::vector<std::vector<std::string>> ID_1;
    std::vector<std::vector<size_t>> ID_locs_1;//ID_locations
    if(gen_id(first, &Text_out_1, &ID_1, &ID_locs_1, showflag, logfile) == 1)
    {
        if(showflag == 1)
            logfile << "Gen ID failed." << endl;
        return 1;
    }

    std::vector<std::string> Text_out_2;
    std::vector<std::vector<std::string>> ID_2;
    std::vector<std::vector<size_t>> ID_locs_2;//ID_locations
    if(gen_id(second, &Text_out_2, &ID_2, &ID_locs_2, showflag, logfile) == 1)
    {
        if(showflag == 1)
            logfile << "Gen ID failed." << endl;
        return 1;
    }

    //generating lists of IDs that have to be described in typedefs
    std::vector<string> typedefs1 = gen_typedefs(ID_locs_1, ID_1), typedefs2 = gen_typedefs(ID_locs_2, ID_2);

    //were unable to genereate typedefs
    if( ( typedefs1.size() > 0 && typedefs1[0] == "FAIL" ) || ( typedefs2.size() > 0 && typedefs2[0] == "FAIL" ) )
    {
        if(showflag == 1)
            logfile << "Could not create additional pseudo-typedefs." << endl;
        return 0;
    }

    //make fragments have same number of left and right figure braces
    braces_balance(&first);
    braces_balance(&second);

    //make fragments look like this:
    //#include <stdio.h>
    //#include <stdlib.h>
    //typedef void* ...;
    //...
    //typedef void* ...;
    //void ast_i_func(){
    //FRAGMENT CODE
    //...
    //return;}
    //which is needed for fragment to be processed with PyCParser
    first.push_back("}");
    first.insert(first.begin(), "void ast1_func(){");
    for(size_t i = 0; i < typedefs1.size(); ++i)
    {
        first.insert(first.begin(), "typedef void* " + typedefs1[i] + ";");
    }
    first.insert(first.begin(), "#include <stdlib.h>");
    first.insert(first.begin(), "#include <stdio.h>");

    second.push_back("}");
    second.insert(second.begin(), "void ast2_func(){");
    for(size_t i = 0; i < typedefs2.size(); ++i)
    {
        second.insert(second.begin(), "typedef void* " + typedefs2[i] + ";");
    }
    second.insert(second.begin(), "#include <stdlib.h>");
    second.insert(second.begin(), "#include <stdio.h>");

    string filename = "CPR4_GCC_PP_C99_AST_1.c";

    write_to_file(filename, first);

    //cout << "equality?" << endl;

    filename = "CPR4_GCC_PP_C99_AST_2.c";

    write_to_file(filename, second);

    //cout << "equality?" << endl;

    exec_git_command("gcc -E -I" + path_to_fake_libc + "fake_libc_include CPR4_GCC_PP_C99_AST_1.c -o out_CPR4_GCC_PP_C99_AST_1.c", showflag, logfile);
    exec_git_command("gcc -E -I" + path_to_fake_libc + "fake_libc_include CPR4_GCC_PP_C99_AST_2.c -o out_CPR4_GCC_PP_C99_AST_2.c", showflag, logfile);

    filename = "CPR4_GCC_PP_C99_AST_1.c";
    remove( filename.c_str() );
    filename = "CPR4_GCC_PP_C99_AST_2.c";
    remove( filename.c_str() );

    vector<string> result;
    exec_command("python3 " + path_to_fake_libc + "testing_file.py out_CPR4_GCC_PP_C99_AST_1.c out_CPR4_GCC_PP_C99_AST_2.c", &result, showflag, logfile);

    filename = "out_CPR4_GCC_PP_C99_AST_1.c";
    remove( filename.c_str() );
    filename = "out_CPR4_GCC_PP_C99_AST_2.c";
    remove( filename.c_str() );

    if(result.size() > 1)
    {
        result.erase( result.begin() + 1, result.end() );
    }

    if(showflag == 1 && result.size() > 0 )
        logfile << result[0] << endl;

    if( result.size() < 1 || result[0].size() < 1 || result[0][0] != '1')
        return 0;

    return 1;
}

//this function will find locations of defects in file PATH and store them in RESULT
int find_defects(string path, char showflag, ofstream& logfile, string path_to_exe, string SHA1)
{
    string filename = path;
    if( filename.find(".c", filename.size() - 2) != std::string::npos )
        filename.erase( filename.rfind(".c") , filename.size() );
    else if( filename.find(".h", filename.size() - 2) != std::string::npos )
        filename.erase( filename.rfind(".h") , filename.size() );
    filename = filename.substr( filename.rfind("/") +1  );
    //for file described by PATH cppcheck is launched, it's result stored in CPR_cppcheck_output.txt
    exec_git_command("cppcheck --std=c99 " + path + " 2> " + path_to_exe + "SHA_1___" + SHA1 + "___" + filename + ".txt", showflag, logfile);

    return 0;
};

#endif // CLUSTER_INIT_H_INCLUDED
