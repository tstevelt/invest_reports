/*----------------------------------------------------------------------------
	Program : rpt_portfolio.c
	Author  : Tom Stevelt
	Date    : 01/24/2019
	Synopsis: A huge variety of portfolio reports.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		03/05/2019	Added -style and -sort.  Move alert to own app.
	tms		03/08/2019	Three versions: base, extended, alerts.  
						Reserve for future custom.
	tms		05/24/2021	Added STYLE_OVERNIGHT
	tms		07/13/2021	Get private api keys with GetInvestCfg().
	tms		04/15/2022	Added Pdate to BASIC
	tms		08/23/2022	Added stock.Sgrow5 field and FUNDAMENTAL report
	tms		01/23/2023	Added Year Over Year performance report.
	tms		07/17/2023	Added -broker option
	tms		02/12/2024	Added Morningstart CSV report

----------------------------------------------------------------------------*/
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

#define		MAIN
#include	"rpt_portfolio.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	Fragment[48];
	char	*AttachArray[1];
	int		rv, TickerColumn;

	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member id %ld\n", ReportMember );
		exit ( 1 );
	}

	/*----------------------------------------------------------
		find date one year ago with history records.
		int DateAdd ( DATEVAL *a, int NumberOfDays, DATEVAL *b )
		long dbySelectCount ( MYSQL *MySql, char *Table, char *WhereClause, char *LogFileName )
	----------------------------------------------------------*/
	CurrentDateval ( &dvToday );
	if ( ReportStyle == STYLE_YOY )
	{
		DateAdd ( &dvToday, -365, &dvYearAgo );
		do
		{
			sprintf ( WhereClause, "Hdate = '%04d-%02d-%02d'", dvYearAgo.year4, dvYearAgo.month, dvYearAgo.day );
			rv = dbySelectCount ( &MySql, "history", WhereClause, LogFileName );
			if ( rv > 100 )
			{
				sprintf ( ascYearAgo, "%04d-%02d-%02d", dvYearAgo.year4, dvYearAgo.month, dvYearAgo.day );
				break;
			}
			DateAdd ( &dvYearAgo, 1, &dvYearAgo );
		} while ( rv < 100 );
	}

	// sprintf ( WhereClause, "Pmember = %ld and Pticker = 'ENB'", ReportMember );
	sprintf ( WhereClause, "Pmember = %ld", ReportMember );

	if ( nsStrlen ( Broker ) > 0 )
	{
		sprintf ( Fragment, " and Pbroker = '%s'", Broker );
		nsStrcat ( WhereClause, Fragment );
	}

	LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) getdata, 1 );

	if ( ReportCount == 0 )
	{
		return ( 0 );
	}

	if ( Debug )
	{
		nsFclose ( fpOutput );
		sprintf ( xbuffer, "cat %s", OutFileName );
		system ( xbuffer );
		return ( 0 );
	}
	nsFclose ( fpOutput );

	sortdata ( OutFileName );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_APPEND_W:
		case RPT_FORMAT_REPLACE_W:
			if ( ReportStyle == STYLE_BASE )
			{
				TickerColumn = 3;
			}
			else if ( ReportStyle == STYLE_YOY )
			{
				TickerColumn = 2;
			}
			else
			{
				TickerColumn = 1;
			}
			rv = ToWatchlist ( &MySql, OutFileName, "|", TickerColumn, ReportOptions.Format, ReportMember, 1 );
			if ( rv < 0 )
			{
				printf ( "ToWatchlist failed, rv %d\n", rv );
			}
			else if ( rv == 0 )
			{
				printf ( "Nothing added to watchlist\n" );
			}
			else
			{
				printf ( "Added %d stocks to watchlist\n", rv );
			}
			break;
		case RPT_FORMAT_TEMP:
			rv = ToTemp ( &MySql, OutFileName, "|", 1, ReportOptions.Format, ReportMember, 1 );
			if ( rv < 0 )
			{
				printf ( "ToTemp failed, rv %d\n", rv );
			}
			else if ( rv == 0 )
			{
				printf ( "Nothing added to temp file\n" );
			}
			else
			{
				printf ( "Added %d stocks to temp file\n", rv );
			}
			break;
		default:
			switch ( ReportStyle )
			{
				case STYLE_BASE:
					dorpt_base ( OutFileName );
					break;
				case STYLE_MSTAR:
					dorpt_mstar ( OutFileName );
					break;
				case STYLE_FUNDAMENTAL:
					dorpt_fundamental ( OutFileName );
					break;
				case STYLE_TODAY:
					dorpt_today ( OutFileName );
					break;
				case STYLE_ALERTS:
					dorpt_alerts ( OutFileName );
					break;
				case STYLE_FULL:
					dorpt_full ( OutFileName );
					break;
				case STYLE_OVERNIGHT:
					dorpt_overnight ( OutFileName );
					break;
				case STYLE_YOY:
					dorpt_yoy ( OutFileName );
					break;
				case STYLE_CUSTOM:
					// dorpt_custom ( OutFileName );
					break;
			}
			break;
	}

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			snprintf ( xbuffer, sizeof(xbuffer), "Portfolio Report - %d stocks", ReportCount);
			AttachArray[0] = ReportOptions.OutputFilename;
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 0, NULL, 1, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_EMAIL:
			snprintf ( xbuffer, sizeof(xbuffer), "Portfolio Report - %d stocks", ReportCount);
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 1, ReportOptions.OutputFilename, 0, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_HTML:
			snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
			system ( xbuffer );
			break;
	}
		
	if ( Debug )
	{
		printf ( "%s\n", OutFileName );
		printf ( "%s\n", ReportOptions.OutputFilename );
		printf ( "%s\n", PRICEFILE );
	}
	else
	{
		unlink ( OutFileName );
		unlink ( ReportOptions.OutputFilename );
		unlink ( PRICEFILE );
	}

	return ( 0 );
}
