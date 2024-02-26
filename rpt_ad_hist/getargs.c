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

#include	"rpt_ad_hist.h"

// ad_hist -1mo -fmt csv   

static void Usage ()
{
	printf ( "USAGE: rpt_ad_hist timeframe -fmt fmt [options]\n" );
	printf ( "timeframe: -1mo -6mo -1yr\n" );
	printf ( "Options:\n" );
	FormatUsage ( 0 );
	printf ( " -member id\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 4 )
	{
		Usage ();
	}

	DateRange = 0;
	ReportMember = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-1mo" ) == 0 )
		{
			DateRange = DATE_RANGE_ONE_MONTH;
		}
		else if ( nsStrcmp ( argv[xa], "-6mo" ) == 0 )
		{
			DateRange = DATE_RANGE_SIX_MONTHS;
		}
		else if ( nsStrcmp ( argv[xa], "-1yr" ) == 0 )
		{
			DateRange = DATE_RANGE_ONE_YEAR;
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
	if ( DateRange == 0 )
	{
		Usage ();
	}
}
