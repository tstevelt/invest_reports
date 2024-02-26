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

#include	"rpt_near52.h"

static int Usage ()
{
	printf ( "USAGE: rpt_near52 index [options]\n" );
	SindexUsage ( 1 );
	printf ( "Options:\n" );
	FormatUsage ( 1 );
	printf ( " -months #   = age of 52 week high (default 3)\n" );
	printf ( " -within #.# = within percent of 52 week high (default 5)\n" );
	printf ( " -dip #.#    = percent dip from 52 week high (default 25)\n" );
	printf ( " -member id\n" );
	printf ( " -one ticker (overrides any index)\n" );
	printf ( " -d\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 4 )
	{
		Usage ();
	}

	StockIndex = toupper ( argv[1][0] );

	ValidStockIndex ( StockIndex, 1, (int(*)()) Usage );
	ReportMember = 0;
	Months = 3;
	Within = 5.0;
	MinimumDip = 25.0;

	Debug = 0;
	for ( xa = 2; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-months" ) == 0 )
		{
			xa++;
			Months = nsAtoi ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-within" ) == 0 )
		{
			xa++;
			Within = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-dip" ) == 0 )
		{
			xa++;
			MinimumDip = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-one" ) == 0 )
		{
			StockIndex = 'x';
			xa++;
			OneStock = argv[xa];
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

}
