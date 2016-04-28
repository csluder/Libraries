/** @file getopt.hpp
 *  @brief Provide Linux/Unix compatability for getopt function
 *  @copyright
 *  Copyright (C) 2014 Charles E Sluder
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *  @author Charles Sluder    - Initial version 09/15/2009
 */

#ifndef GETOPT_H_
#define GETOPT_H_
#include <map>
#include <string>

extern int opterr;
extern int optind;
struct option {
    const char* name;
    int has_arg;
    int *flag;
    int val;
};

extern int optopt;
extern char *optarg;
extern int getopt(int argc, char* const argv[], char* opts);
extern int getopt_long(int argc, char* const argv[], const char *shortopts,
const struct option* longopts, int* longind);

enum FLAG {
    no_argument = 0,
    required_argument,
    optional_argument
};


class GetOpt {
public:


    GetOpt(int argc, char* const argv[], const char *opts, const option* longopts);
    ~GetOpt();

    int getopt( int* longIndex );

    int   optopt;
    char* optarg;
    int   optind;
private:

    struct parms {
        int has_arg;
        int *flag;
        int val;
        int indx;
    };

    
    bool ValidOption( char c ) { return true; }; // XXX fix this someday
    typedef std::map<std::string, parms>::iterator iterator;
    int m_argc;
    int m_nextChar;
    char** m_argv;
    std::map <std::string, parms> m_options;
    iterator getopt_long();
};

extern int getsubopt( char **optionp, char* const* tokens, char **valuep );

#endif