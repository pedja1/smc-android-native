/***************************************************************************
 * file_parser.cpp  -  text file parser
 *
 * Copyright (C) 2005 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../core/global_basic.h"
#include "../core/file_parser.h"
#include "../core/game_core.h"
#include <cstdio>

namespace SMC
{

/* *** *** *** *** *** *** *** *** cFile_parser *** *** *** *** *** *** *** *** *** */

cFile_parser :: cFile_parser( void )
{
	//
}

cFile_parser :: ~cFile_parser( void )
{
	//
}

bool cFile_parser :: Parse( const std::string &filename )
{
	ifstream ifs( filename.c_str(), ios::in );
	
	if( !ifs )
	{
		printf( "Could not load data file : %s\n", filename.c_str() );
		return 0;
	}

	data_file = filename;

	std::string line;
	unsigned int line_num = 0;

	while( std::getline( ifs, line ) )
	{
		line_num++;
		Parse_Line( line, line_num );
	}

	return 1;
}

bool cFile_parser :: Parse_Line( std::string str_line, int line_num )
{
	if( str_line.empty() )
	{
		return 1;
	}

	// linux support
	string_erase_all( str_line, '\r' );
	// no tabs
	string_replace_all( str_line, "\t", " " );
	// remove beginning spaces
	string_trim_from_begin( str_line, ' ' );
	// remove trailing spaces
	string_trim_from_end( str_line, ' ' );

	// ignore empty lines and comments
	if( str_line.empty() || *str_line.begin() == '#' )
	{
		// no error
		return 1;
	}

	std::string tempstr = str_line;
	int count = 1;

	// count spaces
	while( tempstr.find( ' ' ) != std::string::npos  )
	{
		tempstr.erase( tempstr.find( ' ' ) , 1 );
		count++;
	}

	tempstr = str_line;
	
	std::string *parts = new std::string[ count + 1 ];
	
	std::string::size_type len;
	int part_count = 0;

	while( count > 0 )
	{
		// get length of the part
		len = tempstr.find_first_of( ' ' );
		// create part
		parts[part_count] = tempstr.substr( 0, len );
		// remove part from temp string
		tempstr.erase( 0, len + 1 );

		part_count++;
		count--;
	}

	parts[part_count] = tempstr;

	// Message handler
	bool success = HandleMessage( parts, part_count, line_num );

	delete []parts;

	return success;
}

bool cFile_parser :: HandleMessage( const std::string *parts, unsigned int count, unsigned int line )
{
	// virtual
	return 1;
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
