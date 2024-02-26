/*----------------------------------------------------------------------------
	Program : rpt_allocation.c
	Author  : Tom Stevelt
	Date    : D05/15/2019
	Synopsis: Portfolio allocation by Sector or Industry or Both
				Available on the website portfolio page.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		01/06/2023	Added Stype BONDS and CASH.  
						Changed cashbal to Other Cash

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
#include	"rpt_allocation.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	*AttachArray[1];

	getargs ( argc, argv );

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

	switch ( RunMode )
	{
		case MODE_SECTOR:
			dorpt_sector ( OutFileName );
			break;
		case MODE_INDUSTRY:
			dorpt_industry ( OutFileName );
			break;
		case MODE_BOTH:
			dorpt_both ( OutFileName );
			break;
	}

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
			snprintf ( xbuffer, sizeof(xbuffer), "Sec/Ind Allocation Report" );
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
