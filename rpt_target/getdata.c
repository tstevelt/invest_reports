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

#include	"rpt_target.h"

void getdata ()
{
	double	Total = 0.0;

	sprintf ( OutFileName, "%s/allocation_%d.dat", TEMPDIR, getpid() );
	if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
	{
		printf ( "Cannot create %s for output\n", OutFileName );
		exit ( 1 );
	}

	for ( int ndx = 0; ndx < SummaryCount; ndx++ )
	{
		Total += SummaryArray[ndx].Percent;
	}

	if ( Total != 100.0 )
	{
		printf ( "Target allocation (%.2f) does not equal 100%%\n", Total );
		exit ( 1 );
	}

	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) EachPortfolio, 1 );
	
	double	TotalInvest, TotalMarket;
	double	TargetInvest, TargetMarket;
	TargetInvest = TargetMarket = TotalInvest = TotalMarket = 0.0;
	for ( int ndx = 0; ndx < SummaryCount; ndx++ )
	{
		TotalInvest += SummaryArray[ndx].CurrentInvest;
		TotalMarket += SummaryArray[ndx].CurrentMarket;
	}
	for ( int ndx = 0; ndx < SummaryCount; ndx++ )
	{
		TargetInvest += SummaryArray[ndx].TargetInvest = TotalInvest * SummaryArray[ndx].Percent / 100.0;
		TargetMarket += SummaryArray[ndx].TargetMarket = TotalMarket * SummaryArray[ndx].Percent / 100.0;
	}

	double	TotalAdjust = 0.0;
	fprintf ( fpOutput, "T   PCNT       TARGET     INVESTED       TARGET      CUR-MKT       ADJUST\n" );
	for ( int ndx = 0; ndx < SummaryCount; ndx++ )
	{
		fprintf ( fpOutput, "%c %6.2f %12.2f %12.2f %12.2f %12.2f %12.2f\n",
				SummaryArray[ndx].AllocationType,
				SummaryArray[ndx].Percent,
				SummaryArray[ndx].TargetInvest,
				SummaryArray[ndx].CurrentInvest,
				SummaryArray[ndx].TargetMarket,
				SummaryArray[ndx].CurrentMarket,
				SummaryArray[ndx].TargetMarket - SummaryArray[ndx].CurrentMarket );
		TotalAdjust += SummaryArray[ndx].TargetMarket - SummaryArray[ndx].CurrentMarket;
	}
	fprintf ( fpOutput, "         ============ ============ ============ ============ ============\n" );
	fprintf ( fpOutput, "         %12.2f %12.2f %12.2f %12.2f %12.2f\n",
		TargetInvest, TotalInvest, TargetMarket, TotalMarket, TotalAdjust );


	double	SubTotal = 0.0;

	fprintf ( fpOutput, "\n" );
	for ( int ndx = 0; ndx < BrokerCount; ndx++ )
	{
		if ( ndx && strcmp ( BrokerArray[ndx].Broker, BrokerArray[ndx-1].Broker ) != 0 )
		{
			fprintf ( fpOutput, "   %12.12s ------------\n", " " );
			fprintf ( fpOutput, "   %12.12s %12.2f\n\n", " ", SubTotal );
			SubTotal = 0;
		}

		fprintf ( fpOutput, "   %-10.10s %c %12.2f\n", 
			BrokerArray[ndx].Broker,
			BrokerArray[ndx].AllocationType,
			BrokerArray[ndx].Current );
	
		SubTotal += BrokerArray[ndx].Current;
	}

	fprintf ( fpOutput, "   %12.12s ------------\n", " " );
	fprintf ( fpOutput, "   %12.12s %12.2f\n\n", " ", SubTotal );


	/*---------------------------------------------------------------------------
		print bond array, subtotal by year
	---------------------------------------------------------------------------*/
	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Poptexp", &xportfolio, (int(*)()) EachBond, 1 );

	fprintf ( fpOutput, "BONDS BY YEAR\n" );
	fprintf ( fpOutput, "-----------------------------------------------------------------\n" );
	SubTotal = 0.0;
	char	Year[5];
	for ( int ndx = 0; ndx < BondCount; ndx++ )
	{
		if ( ndx == 0 )
		{
			sprintf ( Year, "%4.4s", BondArray[ndx].Expires );
		}
		else if ( strncmp ( Year, BondArray[ndx].Expires, 4 ) != 0 )
		{
			fprintf ( fpOutput, "%52.52s ------------\n", " " );
			fprintf ( fpOutput, "%52.52s %12.2f\n\n", " ", SubTotal );
			sprintf ( Year, "%4.4s", BondArray[ndx].Expires );
			SubTotal = 0;
		}

		fprintf ( fpOutput, "%-10.10s %-30.30s %-10.10s %12.2f\n",
			BondArray[ndx].Ticker,
			BondArray[ndx].Name,
			BondArray[ndx].Expires,
			BondArray[ndx].Value );

		SubTotal += BondArray[ndx].Value;
	}

	fprintf ( fpOutput, "%52.52s ------------\n", " " );
	fprintf ( fpOutput, "%52.52s %12.2f\n\n", " ", SubTotal );

	/*---------------------------------------------------------------------------
		print cash array
	---------------------------------------------------------------------------*/
	fprintf ( fpOutput, "\nCASH\n" );
	fprintf ( fpOutput, "-----------------------------------------------------------------\n" );
	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Pbroker,Pticker", &xportfolio, (int(*)()) EachCash, 1 );

	/*---------------------------------------------------------------------------
		print domestic array
	---------------------------------------------------------------------------*/
	fprintf ( fpOutput, "\nDOMESTIC SECURITIES\n" );
	fprintf ( fpOutput, "-----------------------------------------------------------------\n" );
	MatchDomFor = 'D';
	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Pbroker,Pticker", &xportfolio, (int(*)()) EachStock, 1 );

	/*---------------------------------------------------------------------------
		print foreign array
	---------------------------------------------------------------------------*/
	fprintf ( fpOutput, "\nFOREIGN SECURITIES\n" );
	fprintf ( fpOutput, "-----------------------------------------------------------------\n" );
	MatchDomFor = 'F';
	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Pbroker,Pticker", &xportfolio, (int(*)()) EachStock, 1 );

	fclose ( fpOutput );
}
