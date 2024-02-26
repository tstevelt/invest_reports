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

#include	"rpt_regress.h"


static int Usage ()
{
	printf ( "USAGE: rpt_regress startdate enddate index          [options]\n" );
	printf ( "USAGE: rpt_regress startdate enddate -file filename [options]\n" );
	printf ( "USAGE: rpt_regress startdate enddate -ticker ticker [options]\n" );
	SindexUsage ( 1 );
	printf ( "Options:\n" );
	printf ( " -member id\n" );
	printf ( " -top = top number to report\n" );
	printf ( " -sort = (R)eturn desc (C)orrCoeff desc (V)ariance\n" );
	FormatUsage ( 1 );
	printf ( " -d   = debug\n" );

	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 5 )
	{
		Usage ();
	}

	ReportOptions.Format = RPT_FORMAT_CSV;
	ReportMember = 0;
	Debug = 0;
	StockIndex = '?';
	ReportCount = 10;
	SortBy = SORTBY_RETURN;
	
	StartDate  = argv[1];
	EndDate    = argv[2];

	for ( xa = 3; xa < argc; xa++ )
	{
		if ( argv[xa][0] != '-' && StockIndex == '?' )
		{
			StockIndex = argv[xa][0];
			ValidStockIndex ( StockIndex, 1, (int(*)()) Usage );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-file" ) == 0 )
		{
			xa++;
			StockIndex = 'F';
			InputFileName = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-ticker" ) == 0 )
		{
			xa++;
			StockIndex = 'x';
			Ticker = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-top" ) == 0 )
		{
			xa++;
			ReportCount  = nsAtol ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-sort" ) == 0 )
		{
			xa++;
			switch ( toupper(argv[xa][0]) )
			{
				case 'R':
					SortBy = SORTBY_RETURN;
					break;
				case 'C':
					SortBy = SORTBY_CORREL;
					break;
				case 'V': 
					SortBy = SORTBY_VARIANCE;
					break;
			}
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
				MungeData = 1;
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

	switch ( StockIndex )
	{
		case 'W':
		case 'P':
			if ( ReportMember == 0 )
			{
				printf ( "-member is required for Portfolio or Watchlist\n" );
				Usage ();
			}
			break;
		case '?':
			Usage ();
			break;
	}
}
