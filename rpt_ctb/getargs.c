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

#include	"rpt_ctb.h"

static void Usage ()
{
	printf ( "USAGE: rpt_ctb index [options]\n" );
	SindexUsage ( 1 );
	printf ( "OPTIONS\n" );
	printf ( " -member id   for watchlist or portfolio.  also for email report.\n" );
	printf ( " {-top #|-bot #|-max #.#|-min #.#}] \n" );
	printf ( "   -top #   = top number to report, default 5\n" );
	printf ( "   -bot #   = bottom number to report\n" );
	printf ( "   -min #.# = minimum RS to report, default 0.0\n" );
	printf ( "   -max #.# = maximum RS to report, default 0.0\n" );
	printf ( " -fmt fmt] \n" );
	printf ( " -d]\n" );
	FormatUsage ( 1 );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa, SawMethod;

	ReportOptions.Format = RPT_FORMAT_CSV;
	ReportMember = 0;
	Debug = 0;
	StockIndex = '?';
	ReportCount = 5;
	MinimumRS = 0.0;
	MaximumRS = 0.0;
	SelectStyle = SELECT_TOP;
	SawMethod = 0;

	if ( argc < 2 )
	{
		Usage ();
	}

	StockIndex = toupper ( argv[1][0] );
	ValidStockIndex ( StockIndex, 1, (int(*)()) Usage );

	for ( xa = 2; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-top" ) == 0 )
		{
			xa++;
			ReportCount = nsAtol ( argv[xa] );
			SelectStyle = SELECT_TOP;
			SawMethod++;
		}
		else if ( xa + 1 < argc && nsStrncmp ( argv[xa], "-bot", 4 ) == 0 )
		{
			xa++;
			ReportCount = nsAtol ( argv[xa] );
			SelectStyle = SELECT_BOTTOM;
			SawMethod++;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-min" ) == 0 )
		{
			xa++;
			MinimumRS  = nsAtof ( argv[xa] );
			SelectStyle = SELECT_MINIMUM;
			ReportCount = 0;
			SawMethod++;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-max" ) == 0 )
		{
			xa++;
			MaximumRS  = nsAtof ( argv[xa] );
			SelectStyle = SELECT_MAXIMUM;
			ReportCount = 0;
			SawMethod++;
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

	if ( SawMethod > 1 )
	{
		printf ( "Cannot combine -top and -min\n" );
		Usage ();
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
