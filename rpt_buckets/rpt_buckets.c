/*----------------------------------------------------------------------------
	Program : rpt_buckets.c
	Author  : Tom Stevelt
	Date    : 03/29/2022
	Synopsis: Portfolio allocation by "buckets".
				Available on the website portfolio report page.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		03/30/2022	Added COMPANY NAME, SHARES and GAIN columns
	tms		03/30/2022	Added YIELD column (stock.Syield)
	tms		todo		YIELD should be based on portfolio.Pprice instead
						of stock.Slast. Maybe even two numbers: TTM and 
						since portfolio.Pdate.
	tms		04/01/2022	Added DAYS HELD
	tms		08/28/2022	Moved Yield from stock to fundamental
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
#include	"rpt_buckets.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	*AttachArray[1];

	getargs ( argc, argv );

	CurrentDateval ( &dvToday );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member id %ld\n", ReportMember );
		exit ( 1 );
	}

	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) getdata_portfolio, 1 );

	sprintf ( WhereClause, "CBmember = %ld", ReportMember );
	LoadCashbalCB ( &MySql, WhereClause, "CBacct", &xcashbal, (int(*)()) getdata_cashbal, 1 );

	putdata ();

	if ( ReportCount == 0 )
	{
		return ( 0 );
	}

	if ( Debug )
	{
		sprintf ( xbuffer, "cat %s", OutFileName );
		system ( xbuffer );
		return ( 0 );
	}

	dorpt ( OutFileName );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			snprintf ( xbuffer, sizeof(xbuffer), "Allocation by Buckets Report" );
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
