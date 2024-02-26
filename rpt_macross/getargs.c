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

#include	"rpt_macross.h"

static int Usage ()
{
	printf ( "USAGE: rpt_macross index -avgs # # -days # -type {B|S|A} [options]\n" );
	SindexUsage ( 1 );
	printf ( " -avgs # #  = numbers can be 5, 10, 20, 50, 100 and 200\n" );
	printf ( " -days #    = cross over during past # of days\n" );
	printf ( " -type B|S|A  = looking for Buy, Sell or All\n" );
	FormatUsage ( 1 );
	printf ( "Options:\n" );
	printf ( " -member id\n" );
	printf ( " -d   = Debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 9 )
	{
		Usage ();
	}

	StockIndex = toupper ( argv[1][0] );
	ValidStockIndex ( StockIndex, 1, (int(*)()) Usage );

	ReportMember = 0;
	Days1 = Days2 = CrossOverSince = 0;
	BuySellAll = '?';

	Debug = 0;
	for ( xa = 2; xa < argc; xa++ )
	{
		if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-avgs" ) == 0 )
		{
			xa++;
			Days1 = nsAtoi ( argv[xa] );
			xa++;
			Days2 = nsAtoi ( argv[xa] );
			switch ( Days1 )
			{
				case 5:
					FastAVG = AVG_5;
					break;
				case 10:
					FastAVG = AVG_10;
					break;
				case 20:
					FastAVG = AVG_20;
					break;
				case 50:
					FastAVG = AVG_50;
					break;
				case 100:
					FastAVG = AVG_100;
					break;
				case 200:
					FastAVG = AVG_200;
					break;
				default:
					Usage ();
					break;
			}
			switch ( Days2 )
			{
				case 5:
					SlowAVG = AVG_5;
					break;
				case 10:
					SlowAVG = AVG_10;
					break;
				case 20:
					SlowAVG = AVG_20;
					break;
				case 50:
					SlowAVG = AVG_50;
					break;
				case 100:
					SlowAVG = AVG_100;
					break;
				case 200:
					SlowAVG = AVG_200;
					break;
				default:
					Usage ();
					break;
			}
			if ( Days1 >= Days2 )
			{
				printf ( "Days1 should be less than Days2\n" );
				Usage ();
			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-days" ) == 0 )
		{
			xa++;
			CrossOverSince = nsAtoi ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp (  argv[xa], "-type" ) == 0 )
		{
			xa++;
			BuySellAll  = toupper ( argv[xa][0] );
			switch ( BuySellAll )
			{
				case 'B':
				case 'S':
				case 'A':
					break;
				default:
					Usage ();
					break;
			}

		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
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
		else
		{
			Usage ();
		}
	}

	if ( Days1 == 0 || Days2 == 0 || CrossOverSince == 0 || BuySellAll == '?' )
	{
		Usage ();
	}
}
