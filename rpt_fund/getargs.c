//     Invest report
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include	"rpt_fund.h"


static int Usage ()
{
	printf ( "USAGE: rpt_fund -script filename -member ID [options]\n" );
	printf ( "Options:\n" );
	FormatUsage ( 0 );
	printf ( " -d   = debug\n" );

	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	InputFileName = "";
	ReportOptions.Format = RPT_FORMAT_CSV;
	ReportMember = 0;
	Debug = 0;
	
	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-script" ) == 0 )
		{
			xa++;
			InputFileName = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-fmt" ) == 0 )
		{
			xa++;
			ReportOptions.Format = ValidReportFormatUsage ( argv[xa], (int(*)()) Usage );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			ReportMember = nsAtol ( argv[xa] );
			if ( ReportMember == DEMO_MEMBER )
			{
				srand ( time ( NULL ) );
			}
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			Usage ();
		}
	}

	if ( nsStrlen(InputFileName) < 1 )
	{
		Usage ();
	}
}
