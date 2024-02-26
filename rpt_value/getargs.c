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

#include	"rpt_value.h"

static void Usage ()
{
	printf ( "USAGE: rpt_value -member # [-currentAAA #.#] [-index x] [-range #.#] [options]\n" );
	printf ( "options:\n" );
	printf ( " -currentAAA #.# current yield of AAA corp bonds, default 3.0 percent\n" );
	printf ( "                 https://fred.stlouisfed.org/series/AAA\n" );
	printf ( " -index x, default is A, can also use P or W\n" );
	printf ( " -range #.#, percent for Buy/Hold/Sell, default is 15.0 percent\n" );
	printf ( " -fmt {csv*|txt|html|email|xlsx\n" );
	printf ( " -d   = debug, prints alert or report, does not send email\n" );
	printf ( " * default value\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	StockIndex = 'A';
	RangePercent = 15.0;
	ReportMember = 0;
	CurrentYieldAAA = 3.0;

	if ( argc < 3 )
	{
		Usage ();
	}
	
	ReportOptions.Format = RPT_FORMAT_CSV;
	RunMode = MODE_REPORT;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( xa + 1 < argc &&  nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			ReportMember = nsAtol ( argv[xa] );
//			if ( ReportMember == DEMO_MEMBER )
//			{
//				srand ( time ( NULL ) );
//				MungeData = 1;
//			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-range" ) == 0 )
		{
			xa++;
			RangePercent = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-currentAAA" ) == 0 )
		{
			xa++;
			CurrentYieldAAA = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-fmt" ) == 0 )
		{
			xa++;
			ReportOptions.Format = ValidReportFormatUsage ( argv[xa], (int(*)()) Usage );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-index" ) == 0 )
		{
			xa++;
			StockIndex = toupper ( argv[xa][0] );
			switch ( StockIndex )
			{
				case 'D':
				case 'S':
				case 'N':
				case '1':
				case '2':
				case '3':
				case 'P':
				case 'W':
					break;
				default:
					printf ( "Only index are D S N 1 2 3 P or W\n" );
					Usage ();
			}
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}

	if ( ReportMember == 0 )
	{
		printf ( "-member is required\n" );
		Usage ();
	}
}
