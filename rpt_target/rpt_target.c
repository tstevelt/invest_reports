/*----------------------------------------------------------------------------
	Program : rpt_target.c
	Author  : Tom Stevelt
	Date    : 02/03/2024
	Synopsis: Current Portfolio Asset Allocation vs Target Allocation
				Available on the website portfolio page.

	To Do   : Add target percentages to member table and update from profile page.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		02/11/2024	Add asset type section for cash, domestic and foreign
	tms		02/11/2024	Add to website and Finish delivery methods.
	tms		04/18/2024	Added target allocation fields to table invest.member

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
#include	"rpt_target.h"

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

	/*----------------------------------------------------------
		temporary.  load from fields to be added to member.
		est_target 1000000 300000 25000 0.85
		CASH       2.5
		BOND      30.0
		DOMESTIC  57.4
		FOREIGN   10.1
		McashT,MbondT,MdomT,MforT
xxx	SummaryArray[0].Percent         = 2.5;
xxx	SummaryArray[1].Percent         = 30.0;
xxx	SummaryArray[2].Percent         = 57.4;
xxx	SummaryArray[3].Percent         = 10.1;
	----------------------------------------------------------*/
	SummaryArray[0].AllocationType = CASH;
	SummaryArray[0].Percent        = xmember.xmcasht;
	SummaryArray[1].AllocationType = BOND_AND_CD;
	SummaryArray[1].Percent        = xmember.xmbondt;
	SummaryArray[2].AllocationType = DOMESTIC_EQUITY;
	SummaryArray[2].Percent        = xmember.xmdomt;
	SummaryArray[3].AllocationType = FOREIGN_EQUITY;
	SummaryArray[3].Percent        = xmember.xmfort;
	SummaryCount = 4;

	getdata ();

	dorpt ( OutFileName );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_TEXT:
			snprintf ( xbuffer, sizeof(xbuffer), "Asset Allocation Report" );
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
