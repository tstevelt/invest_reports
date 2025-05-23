/*----------------------------------------------------------------------------
	Program : rpt_porthist.c
	Author  : Tom Stevelt
	Date    : 04/20/2024
	Synopsis: Print porthist report

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		05/14/2025	Reverse sort and stop when we find high value

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
#include	"rpt_porthist.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	*AttachArray[1];
	DATEVAL	dvDate;

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member id %ld\n", ReportMember );
		exit ( 1 );
	}

	CurrentDateval ( &dvDate );
	DateAdd ( &dvDate, -730, &dvDate );
	sprintf ( WhereClause, "PHmember = %ld and PHdate >= '%04d-%02d-%02d'", 
					ReportMember, dvDate.year4, dvDate.month, dvDate.day  );
	
	LoadPorthistCB ( &MySql, WhereClause, "PHdate", &xporthist, (int(*)()) getdata, 0 );

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

	sortdata ();

	dorpt ( OutFileName );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			snprintf ( xbuffer, sizeof(xbuffer), "Portfolio Performance  Report - %d days", ReportCount);
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
