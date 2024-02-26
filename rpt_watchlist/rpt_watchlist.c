/*----------------------------------------------------------------------------
	Program : rpt_watchlist.c
	Author  : Tom Stevelt
	Date    : 01/24/2019
	Synopsis: Print watchlist report, with either basic or fundamental data.
				Available on the website portfolio page.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		03/05/2019	Added -style and -sort.  Move alert to own app.

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
#include	"rpt_watchlist.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	int     (*ptrFunc)();
	char	*AttachArray[1];

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member id %ld\n", ReportMember );
		exit ( 1 );
	}

	sprintf ( WhereClause, "Wmember = %ld", ReportMember );

	switch ( ReportStyle )
	{
		case STYLE_BASE:
			ptrFunc = EachBasic;
			break;
		case STYLE_FUNDAMENTAL:
			ptrFunc = EachFundamental;
			break;
	}

	LoadWatchlistCB ( &MySql, WhereClause, "Wticker", &xwatchlist, (int(*)()) ptrFunc, 1 );

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

	switch ( ReportStyle )
	{
		case STYLE_BASE:
			dorpt_basic ( OutFileName );
			break;
		case STYLE_FUNDAMENTAL:
			dorpt_fundamental ( OutFileName );
			break;
	}

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			snprintf ( xbuffer, sizeof(xbuffer), "Watchlist Report - %d stocks", ReportCount);
			AttachArray[0] = ReportOptions.OutputFilename;
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 0, NULL, 1, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_HTML:
			snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
			system ( xbuffer );
			break;
	}
		
	unlink ( OutFileName );
	unlink ( ReportOptions.OutputFilename );

	return ( 0 );
}
