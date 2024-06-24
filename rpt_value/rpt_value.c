/*----------------------------------------------------------------------------
	Program : rpt_value.c
	Author  : Tom Stevelt
	Date    : 08-19-2023
	Synopsis: Report intrinsic values (Graham and modern), margin of safety 
				and current price.  Available on the website screeners page.

	Who		Date		Modification
	---------------------------------------------------------------------

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
#include	"rpt_value.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	*AttachArray[1];

	getargs ( argc, argv );

//	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member id %ld\n", ReportMember );
		exit ( 1 );
	}

	sprintf ( WhereClause, "(Stype = 'S' or Stype = 'A')" );
	switch ( StockIndex )
	{
		case 'D':
			nsStrcat ( WhereClause, " and Sdj = 'Y'" );
			LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
			break;
		case 'S':
			nsStrcat ( WhereClause, " and Ssp500 = 'Y'" );
			LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
			break;
		case 'N':
			nsStrcat ( WhereClause, " and Snasdaq = 'Y'" );
			LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
			break;
		case '1':
			nsStrcat ( WhereClause, " and Srussell = '1'" );
			LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
			break;
		case '2':
			nsStrcat ( WhereClause, " and Srussell = '2'" );
			LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
			break;
		case '3':
			nsStrcat ( WhereClause, " and (Srussell = '1' or Srussell = '2')" );
			LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
			break;
		case 'P':
			sprintf ( WhereClause, "Pmember = %ld", xmember.xid );
			LoadPortfolioCB ( &MySql, WhereClause, NULL, &xportfolio, (int(*)()) EachPortfolio, 0 );
			break;
		case 'W':
			sprintf ( WhereClause, "Wmember = %ld", xmember.xid );
			LoadWatchlistCB ( &MySql, WhereClause, NULL, &xwatchlist, (int(*)()) EachWatchlist, 0 );
			break;
	}

	if ( ReportCount == 0 )
	{
		printf ( "Nothing to report, missing necessary data.\n" );
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

	dorpt ( OutFileName );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			snprintf ( xbuffer, sizeof(xbuffer), "Intrinsic Value Report - %d stocks", ReportCount);
			AttachArray[0] = ReportOptions.OutputFilename;
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 0, NULL, 1, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_EMAIL:
			snprintf ( xbuffer, sizeof(xbuffer), "Intrinsic Value Report - %d stocks", ReportCount);
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 1, ReportOptions.OutputFilename, 0, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_HTML:
			snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
			system ( xbuffer );
			break;
	}
		
//	unlink ( OutFileName );
//	unlink ( ReportOptions.OutputFilename );

	return ( 0 );
}
