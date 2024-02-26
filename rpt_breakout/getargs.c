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

#include	"rpt_breakout.h"

static int Usage ()
{
	printf ( "USAGE: rpt_breakout index [options]\n" );
	SindexUsage ( 1 );
	printf ( "Options:\n" );
	printf ( " -member id\n" );
	FormatUsage ( 1 );
	printf ( " -price #.#   Percent Increase (default 2)\n" );
	printf ( " -volume #    Percent Increase (default 25)\n" );
	printf ( " -minvol #    Minimum Avg Vol  (default 300000)\n" );
	printf ( " -d\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	StockIndex = toupper ( argv[1][0] );
	ValidStockIndex ( StockIndex, 1, (int(*)()) Usage );

	Debug = 0;
	PriceCutoff = 2.0;
	VolumeCutoff = 25.0;
	MinimumAvgVol = 300000;

	for ( xa = 2; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-price" ) == 0 )
		{
			xa++;
			PriceCutoff = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-volume" ) == 0 )
		{
			xa++;
			VolumeCutoff = nsAtof ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-minvol" ) == 0 )
		{
			xa++;
			MinimumAvgVol = nsAtof ( argv[xa] );
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
