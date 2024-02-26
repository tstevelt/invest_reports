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

#include	"rpt_portfolio.h"

static void Usage ()
{
	printf ( "USAGE: rpt_portfolio -member id [options]\n" );
	printf ( "options:\n" );
	printf ( " -style {base*|mstar|full|fundamental|alerts|overnight|today|yoy}\n" );
	printf ( " -type {all*|stock|etf|fixed|cash}\n" );
	printf ( " -broker broker/account\n" );
	printf ( " -sort {ticker*|gain|roi|compared|}\n" );
	printf ( " -fmt {csv*|txt|html|email|xlsx|others not available\n" );
	printf ( " -d   = debug, prints alert or report, does not send email\n" );
	printf ( " * default value\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa, xi, xl;

	ReportMember = 0;

	if ( argc < 3 )
	{
		Usage ();
	}
	else if ( nsStrcmp ( argv[1], "-member" ) == 0 )
	{
	 	ReportMember = nsAtol ( argv[2] );
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
	
	ReportOptions.Format = RPT_FORMAT_CSV;
	RunMode = MODE_REPORT;
	ReportStyle = STYLE_BASE;
	SortBy = SORTBY_TICKER;
	StockType = 'A';

	for ( xa = 3; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-fmt" ) == 0 )
		{
			xa++;
			ReportOptions.Format = ValidReportFormatUsage ( argv[xa], (int(*)()) Usage );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-style" ) == 0 )
		{
			xa++;
			if ( nsStrcmp ( argv[xa], "base" ) == 0 )
			{
				ReportStyle = STYLE_BASE;
			}
			else if ( nsStrcmp ( argv[xa], "mstar" ) == 0 )
			{
				ReportStyle = STYLE_MSTAR;
			}
			else if ( nsStrcmp ( argv[xa], "full" ) == 0 )
			{
				ReportStyle = STYLE_FULL;
			}
			else if ( nsStrcmp ( argv[xa], "fundamental" ) == 0 )
			{
				ReportStyle = STYLE_FUNDAMENTAL;
			}
			else if ( nsStrcmp ( argv[xa], "alerts" ) == 0 )
			{
				ReportStyle = STYLE_ALERTS;
			}
			else if ( nsStrcmp ( argv[xa], "overnight" ) == 0 )
			{
				ReportStyle = STYLE_OVERNIGHT;
			}
			else if ( nsStrcmp ( argv[xa], "today" ) == 0 )
			{
				ReportStyle = STYLE_TODAY;
			}
			else if ( nsStrcmp ( argv[xa], "yoy" ) == 0 )
			{
				ReportStyle = STYLE_YOY;
			}
//			else if ( nsStrcmp ( argv[xa], "custom" ) == 0 )
//			{
//				ReportStyle = STYLE_CUSTOM;
//				printf ( "Custom -style not implemented\n" );
//				Usage ();
//			}
			else
			{
				printf ( "Unknown -style\n" );
				Usage ();
			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-type" ) == 0 )
		{
			// printf ( " -type {all*|stock|etf|fixed|cash}\n" );
			xa++;
			if ( nsStrcmp ( argv[xa], "all" ) == 0 )
			{
				StockType = 'A';
			}
			else if ( nsStrcmp ( argv[xa], "stock" ) == 0 )
			{
				StockType = 'S';
			}
			else if ( nsStrcmp ( argv[xa], "etf" ) == 0 )
			{
				StockType = 'E';
			}
			else if ( nsStrcmp ( argv[xa], "fixed" ) == 0 )
			{
				StockType = 'F';
			}
			else if ( nsStrcmp ( argv[xa], "cash" ) == 0 )
			{
				StockType = 'C';
			}
			else
			{
				printf ( "Unknown -type\n" );
				Usage ();
			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-broker" ) == 0 )
		{
			xa++;
			xl = sprintf ( Broker, "%s", argv[xa] );
			for ( xi = 0; xi < xl; xi++ )
			{
				if ( Broker[xi] == '_' )
				{
					Broker[xi] = ' ';
				}
			}

		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-sort" ) == 0 )
		{
			xa++;
			if ( nsStrcmp ( argv[xa], "ticker" ) == 0 )
			{
				SortBy = SORTBY_TICKER;
			}
			else if ( nsStrcmp ( argv[xa], "gain" ) == 0 )
			{
				SortBy = SORTBY_GAIN;
			}
			else if ( nsStrcmp ( argv[xa], "roi" ) == 0 )
			{
				SortBy = SORTBY_RETURN;
			}
			else if ( nsStrcmp ( argv[xa], "compared" ) == 0 )
			{
				SortBy = SORTBY_COMPARED;
			}
			else
			{
				printf ( "Unknown -sort\n" );
				Usage ();
			}
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

	if ( ReportStyle == STYLE_MSTAR )
	{
		switch ( ReportOptions.Format )
		{
			case RPT_FORMAT_CSV:
			case RPT_FORMAT_HTML:
				break;
			default:
				if ( Debug )
				{
					printf ( "SETTING RPEORT FORMAT TO CSV\n" );
				}
				ReportOptions.Format = RPT_FORMAT_CSV;
				break;
		}
	}

	switch ( SortBy )
	{
		case SORTBY_TICKER:
		case SORTBY_RETURN:
			break;
		case SORTBY_GAIN:
			switch ( ReportStyle )
			{
				case STYLE_ALERTS:
				case STYLE_MSTAR:
				case STYLE_FUNDAMENTAL:
					printf ( "Cannot sort by gain in alert report.\n" );
					Usage ();
					break;
			}
			break;
		case SORTBY_COMPARED:
			switch ( ReportStyle )
			{
				case STYLE_BASE:
				case STYLE_MSTAR:
				case STYLE_FUNDAMENTAL:
				case STYLE_FULL:
				case STYLE_TODAY:
				case STYLE_YOY:
					printf ( "Cannot sort by comparison in this report.\n" );
					Usage ();
					break;
			}
			break;
	}

}
