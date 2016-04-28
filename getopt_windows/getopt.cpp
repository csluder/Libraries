/** @file getopt.cpp
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
 *  @bugs 
 *      -Cannot handle collisons between short and long options
 *
 *  @author Charles Sluder    - Initial version 09/15/2009
 */

#include <getopt.h>
#include <iostream>
#include <list>

char* optarg;
int opterr = 1;
int optind = 1;
int optopt;

int 
getopt(int argc, char* const argv[], char *opts)
{
    int rOpt = '?';
    static GetOpt* pGetopt = NULL;

    if ( (pGetopt == NULL) || (optind == 0) ) {
        if (pGetopt != NULL) {
            delete pGetopt;
        }
        pGetopt = new GetOpt( argc, argv, opts, (const struct option*)NULL );
    }

    rOpt = pGetopt->getopt( NULL );

    if (rOpt == '?' && opterr != 0) {
        std::cerr << "Unrecogniced option" << optopt << std::endl;
    }

    optarg = pGetopt->optarg;
    optind = pGetopt->optind;
    optopt = pGetopt->optopt;

    if (rOpt == 0) {
        delete pGetopt;
        pGetopt = NULL;
    }

    return rOpt;
}

int
getopt_long( int argc, char* const argv[], const char *shortopts,
    const struct option* longopts, int* longind )
{
    int rOpt = '?';
    static GetOpt* pGetopt = NULL;

    if ((pGetopt == NULL) || (optind == 0)) {
        if (pGetopt != NULL) {
            delete pGetopt;
        }
        pGetopt = new GetOpt( argc, argv, shortopts, (const struct option*)NULL );
    }

    rOpt = pGetopt->getopt( NULL );

    if (rOpt == '?' && opterr != 0) {
        std::cerr << "Unrecogniced option" << optopt << std::endl;
    }

    optarg = pGetopt->optarg;
    optind = pGetopt->optind;
    optopt = pGetopt->optopt;

    if ( rOpt == -1 ) {
        delete pGetopt;
        pGetopt = NULL;
    }

    return rOpt;
}


GetOpt::GetOpt(int argc, char* const argv[], const char *opts, const option* longopts)
{

    m_argc = argc;
    m_argv = (char**)argv;
    optind = 1;
    m_nextChar = 0;

    optarg = argv[optind];
    
    int len = strlen(opts);
    if ( len > 0 ) {
        std::string sOpt;
        parms args;

        // Stash all of the short options into a map for faster and easier loopup.
        for (int i = 0; i < len;) {
            char opt = opts[ i++ ];
            if  ( ValidOption( opt ) ) {
                if ( i < len && opts[ i ] == ':' ) {
                    if ( ++i < len && opts[ i ] == ':' ) {
                        args.has_arg = optional_argument;
                        i++;
                    } 
                    args.has_arg = required_argument;
                } else {
                    args.has_arg = no_argument;
                }
           
                args.flag = NULL;
                args.val = opt;
            
                sOpt = opt;
                m_options.insert( std::pair< std::string, parms>( sOpt, args ) );
            }
        }
    }

    // Stash all of the long options into the map.
    int indx = 0;
    if ( longopts != NULL ) {
        parms args;
        while (longopts->name != 0) {
            args.flag = longopts->flag;
            args.val = longopts->val;
            if (longopts->has_arg >= no_argument && longopts->has_arg <= optional_argument) {
                args.has_arg = longopts->has_arg;
            } else {
                args.has_arg = 0;
            }
            args.indx = indx;

            m_options.insert( std::pair< std::string, parms>( longopts->name, args ) );
        }
    }

}

GetOpt::~GetOpt()
{
    m_options.erase( m_options.begin(), m_options.end() );
}

GetOpt::iterator GetOpt::getopt_long()
{
    m_nextChar++;
    char* current = m_argv[ optind ];
    iterator it;

    std::string needle;

    needle = &current[ m_nextChar ];
    int offset = needle.find( '=' );
    if ( offset == std::string::npos ) {
        optind++;
        m_nextChar = 0;
        current = m_argv[ optind ];
    } else {
        m_nextChar += (offset + 1);
        needle = needle.substr( 0, offset - 1 );
    }

    it = m_options.lower_bound( needle );

    if ( it != m_options.end() ) {
        // check if exact match or substring. If substring than check next
        // container to verify the needle is not a substring of it.
        if (needle.compare( it->first ) != 0) {
            iterator itnext = it;
            itnext++;

            if (itnext->first.compare( needle ) == 0) {
                it = m_options.end();
            }
        }
    }
     return it;
}

int
GetOpt::getopt( int* longIndex )
{
    char* current = m_argv[optind];
    std::string needle;
    iterator it;
    this->optarg = NULL;
    this->optopt = 0;

    // This case happens when there are no non-option arguments or
    // when the argv data has been reordered.
    if ( this->optind == m_argc ) {
        return -1;
    }

    // To emulate gcc behavior non-option arguments need to be shifted to the
    // end of the argv array. 
    if ( m_nextChar == 0 && current[ 0 ] != '-' ) {
        char* ptr = m_argv[ this->optind ];
        int indx = this->optind;
        int next = this->optind;
        std::list<char*> fifo;

        fifo.push_back( m_argv[ indx ] );
        while (++indx != m_argc) {
            if (m_argv[ indx ][ 0 ] != '-') {
                fifo.push_back( m_argv[ indx ] );
            } else {
                m_argv[ next++ ] = m_argv[ indx ];
            }
        }

        // If reordering of pointers occurred then all options have been
        // processed.
        if (next == this->optind) {
            fifo.erase( fifo.begin(), fifo.end() );
            return -1;
        }
        
        while (!fifo.empty()) {
            m_argv[ next++ ] = fifo.front();
            fifo.pop_front();

            // Change the internal argc so that we don't need to run through
            // the reordered data again after the options are processed.
            m_argc--;
        }

        // Reset current to the new value at optind
        current = m_argv[ this->optind ];
    }
        
    if ( m_nextChar != 0 ) {
        // In the middle of an option string
        needle = &current[ m_nextChar ];
        it = m_options.find( needle );

    } else if ( current[ 0 ] == '-' ) {
        // Start of a new option string
        if (current[ 1 ] != '-') {
            // Get an iterator for a short option.
            needle = &current[ ++m_nextChar ];
            it = m_options.find( needle );
        } else {
            // Get an iterator for the long option.
            m_nextChar += 2;
            it = getopt_long();
        }
    }

    // Unrecognized option
    if ( it == m_options.end() ) {
        if ( !needle.empty() ) {
            this->optopt = needle.at( 0 );
        }
        return '?';
    }

    if ( longIndex != NULL ) {
        *longIndex = it->second.indx;
    }

    if ( current[ ++m_nextChar ] == 0 ) {
        m_nextChar = 0;
        this->optind++;
        if ( this->optind < m_argc ) {
            current = m_argv[ this->optind ];
        }
    }


    //Process optional and required arguments
    if ( it->second.has_arg != no_argument ) {
        if ( optind == m_argc || current[ m_nextChar ] == '-' ) {
            if ( it->second.has_arg == required_argument ) {
                if ( !needle.empty() ) {
                    optopt = needle.at( 0 );
                }
                return '?';
            }        
        }

        optarg = &current[ m_nextChar ];
        this->optind++;
        m_nextChar = 0;
    }

    if ( it->second.flag != NULL ) {
        *(it->second.flag) = it->second.val;
        return 0;
    }

    return it->second.val;
}