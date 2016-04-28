/** @file getsubopt.cpp
 *  @brief Provide Linux/Unix compatability for getsubopt function
 *  @copyright
 *  Copyright (C) 2014 Charles E Sluder
 *  Example application using the signing class.  This application can sign a file using
 *  either an embedded key or a PEM file.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *  @bugs
 *  @author Charles Sluder    - Initial version 09/17/2009
 */

#include <getopt.h>
#include <iostream>
#include <cstring>


/**
 * @brief Provide Unix compatability for Operating systems that don't support
 *        getsubopt.
 *
 * Got tired of trying to port Unix code to Windows and not having any option
 * processing capabilities. The only difference I know about is that I don't
 * require a Token array since I usually use don't use it and being forced to
 * include one annoys me.
 */
int
getsubopt( char** subopts, char* const* tokens, char** value )
{
    // Check that params are valid.  GNU C requires the token pointer, but
    // that isn't always needed or wanted. So don't fail if it isn't passed.
    if ( value == NULL ) {
        return -1;
    }

    // Check if parsing of option string is complete
    if (*subopts[ 0 ] == '\0') {
        return -1;
    }
 
    // If we don't find a token then we are going to return everything upto the
    // next suboption delimiter
    *value = *subopts;

    // Find the delimiters for the value and next suboption.
    char* pValue = strchr( *subopts, '=' );
    char* pNext = strchr( *subopts, ',' );

    // If there was a value delimiter set the value to point to it.
    if (pValue != NULL) {
        // Set delimiter to 0 to create token used for searching token array.
        // Don't destroy pValue since the delimiter may need to be restored if
        // the token isn't found in the array.
        *pValue = '\0';
    } 


    // Assuming the delimiter is the end of the first suboption. Replace it with 0
    // to create the token or value string. 
    if ( pNext != NULL ) {
        *pNext++ = '\0';
    } else {
        // Finished, set subopts to point to NULL at end of string
        pNext = *subopts + strlen( *subopts );
    }
    

    if (tokens != NULL) {
        for (int i = 0; tokens[ i ] != NULL; ++i) {
            if (strcmp( *subopts, tokens[ i ] ) == 0) {
                // If delimiter was found then advance pointer to point to
                // start of value string.
                if ( pValue != NULL ) pValue++;
                *value = pValue;
                return i;
            }
        }
    }
    
    // Not found. Restore and return entire suboption string.
    if (pValue != NULL) {
        *pValue = '=';
    }


    //Stage subopts for the next call
    *subopts = pNext;

    return -1;
}

