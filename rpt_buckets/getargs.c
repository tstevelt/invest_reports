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

#include	"rpt_buckets.h"

static void Usage ()
{
	printf ( "USAGE: rpt_buckets -member id [options]\n" );
	printf ( "options:\n" );
	printf ( " -fmt {csv*|txt|html|pdf (n/a)|xlsx}\n" );
	printf ( " -d   = debug, prints alert or report, does not send email\n" );
	printf ( " * default value\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	ReportMember = 0;

	if ( argc < 3 )
	{
		Usage ();
	}
	
	ReportOptions.Format = RPT_FORMAT_CSV;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			ReportMember = nsAtol ( argv[xa] );
			if ( ReportMember == DEMO_MEMBER )
			{
				srand ( time ( NULL ) );
				MungeData = 1;
			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-type" ) == 0 )
		{
			xa++;
			continue;
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
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-sort" ) == 0 )
		{
			xa++;
			continue;
		}
		else
		{
			printf ( "Unknown arg %s\n", argv[xa] );
			exit ( 1 );
		}
	}

	if ( ReportMember == 0 )
	{
		printf ( "-member is required\n" );
		Usage ();
	}
}
