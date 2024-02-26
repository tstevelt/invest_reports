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

#include	"rpt_volume.h"

static int Usage ( char *Msg )
{
	if ( Msg && nsStrlen ( Msg ) )
	{
		printf ( "%s\n", Msg );
	}

	printf ( "USAGE: rpt_volume type index          [options]\n" );
	printf ( "USAGE: rpt_volume type -file filename [options]\n" );
	printf ( "USAGE: rpt_volume type -ticker TICKER [options]\n" );
	printf ( "type:\n" );
	printf ( " I = Increasing volume\n" );
	printf ( " A = Accumulate exceeds Distribution\n" );
	SindexUsage ( 1 );
	printf ( "Options:\n" );
	printf ( " -member id\n" );
	printf ( " -pcnt pcnt = report above this percentage\n" );
	printf ( " -vol vol = minimum volume\n" );
	printf ( " -days curr = number of trading days\n" );
	printf ( " -compare prev curr = number of trading days compare current to previous\n" );
	FormatUsage ( 1 );
	printf ( " -d = debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa, xi;

	if ( argc < 3 )
	{
		Usage ( "ARG COUNT" );
	}

	ReportType = '?';
	StockIndex = '?';
	ReportOptions.Format = RPT_FORMAT_CSV;
	ReportMember = 0;
	TopPercent = 120.0;
	MinimumVolume = 250000;
	NumberOfDays = 10;
	NumberOfDaysPrevious = 45;
	NumberOfDaysCurrent = 5;
	Debug = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( ReportType == '?' )
		{
			ReportType = toupper ( argv[xa][0] );
			switch ( ReportType )
			{
				case 'I':
				case 'A':
					break;
				default:
					Usage ( "REPORT TYPE" );
					break;
			}
		}
		else if ( argv[xa][0] != '-' &&  StockIndex == '?' )
		{
			StockIndex = toupper ( argv[xa][0] );
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
			Ticker = argv[xa];
			StockIndex = 'x';
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
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-pcnt" ) == 0 )
		{
			xa++;
			TopPercent = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-vol" ) == 0 )
		{
			xa++;
			MinimumVolume = nsAtol ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-days" ) == 0 )
		{
			xa++;
			NumberOfDays = nsAtoi ( argv[xa] );
		}
		else if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-compare" ) == 0 )
		{
			xa++;
			NumberOfDaysPrevious = nsAtoi ( argv[xa] );
			xa++;
			NumberOfDaysCurrent = nsAtoi ( argv[xa] );
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			printf ( "UNKNOWN ARG [%s]\n", argv[xa] );
			for ( xi = 0; xi < nsStrlen(argv[xa]); xi++ )
			{
				printf ( " %d=%d", xi, argv[xa][xi] );
			}
			exit ( 1 );
			// Usage ( "UNKNOWN ARG" );
		}
	}

	if ( ReportType == '?' || StockIndex == '?' )
	{
		Usage ( "MISSING REPORT TYPE OR STOCK INDEX" );
	}

	switch ( StockIndex )
	{
		case 'W':
		case 'P':
			if ( ReportMember == 0 )
			{
				Usage ( "NEED MEMBRER FOR WATCHLIST OR PORTFOLIO" );
			}
			break;
	}
}
