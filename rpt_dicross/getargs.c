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

#include    "rpt_dicross.h"

static int Usage ()
{
	printf ( "USAGE: rpt_dicross -index  index  -member ID [options]\n" );
	printf ( "USAGE: rpt_dicross -ticker TICKER -member ID [options]\n" );
	SindexUsage ( 1 );
	printf ( " -member id\n" );
	FormatUsage ( 1 );
	printf ( "Options:\n" );
	printf ( " -days #       = cross over during past # of days, default 1\n" );
	printf ( " -buyadx #     = minimum ADX for buy, default 30.0\n" );
	printf ( " -buyamt #     = Dollar to buy, default 1000.0\n" );
	printf ( " -minshares #  = Minimum shares to buy. Default 10\n" );
	printf ( " -d   = Debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 3 )
	{
		Usage ();
	}

	StockIndex = '?';
	ReportMember = 0;
	CrossOverSince = 1;
	MinimumADX = 30.0;
	BuyAmount = 1000.0;
	MinShares = 10;
	MaxPrice = BuyAmount / (double) MinShares;
	Debug = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-index" ) == 0 )
		{
			xa++;
			StockIndex = toupper(argv[xa][0]);
			SetWhereClause ( WhereClause, StockIndex );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-ticker" ) == 0 )
		{
			xa++;
			sprintf ( WhereClause, "Sticker = '%s'", argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-days" ) == 0 )
		{
			xa++;
			CrossOverSince = nsAtoi ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-buyadx" ) == 0 )
		{
			xa++;
			MinimumADX = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-buyamt" ) == 0 )
		{
			xa++;
			BuyAmount = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-minshares" ) == 0 )
		{
			xa++;
			MinShares = nsAtoi ( argv[xa] );
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

	if ( StockIndex == '?' || ReportMember == 0 )
	{
		Usage ();
	}

	MaxPrice = BuyAmount / (double) MinShares;
}
