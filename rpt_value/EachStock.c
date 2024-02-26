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

#include	"rpt_value.h"

static	double	GrahamValue = 0.0;
static	char	*GrahamFlag = "?";
static	double	ModernValue = 0.0;
static	char	*ModernFlag = "?";

#define		GRAHAM_METHOD	'G'
#define		MODERN_METHOD	'M'

static double IntrinsicValue ( char Method )
{
	static	double	rv;
	double	EPS;
	
	if ( xfundamental.xfepsttm > 0.0 )
	{
		EPS = xfundamental.xfepsttm;
	}
	else if ( xfundamental.xfepsfwd > 0.0 )
	{
		EPS = xfundamental.xfepsfwd;
	}
	else
	{
		EPS = 0.0;
	}

	switch ( Method )
	{
		case GRAHAM_METHOD:
			rv = ( EPS * ( 8.5 + 2 * xfundamental.xfgrow5 ) * 4.4 ) / CurrentYieldAAA;
			break;

		case MODERN_METHOD:
			rv = ( EPS * ( 7.0 + xfundamental.xfgrow5 ) * 4.4 ) / CurrentYieldAAA;
			break;
		
		default:
			fprintf ( stderr, "IntrinsicValue: unknown method\n" );
			exit ( 1 );
	}
	return ( rv );
}

static char *CalcBuyHoldSell ( double IntrinsicValue )
{
	double		BuyValue, SellValue;
	
	BuyValue  = IntrinsicValue * ( 100.0 - RangePercent ) / 100.0;
	SellValue = IntrinsicValue * ( 100.0 + RangePercent ) / 100.0;

	if ( xstock.xsclose < BuyValue )
	{
		return ( "BUY" );
	}
	else if ( xstock.xsclose > SellValue )
	{
		return ( "SELL" );
	}
	return ( "HOLD" );
}

int EachStock ()
{
	int		rv;

	sprintf ( WhereClause, "Fticker = '%s'", xstock.xsticker );
	if (( rv = LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 )) != 1 )
	{
		fprintf ( stderr, "LoadFundamental %s returned %d\n", xstock.xsticker, rv );
		ErrorCount++;
		return ( 0 );
	}
	
	if ( xfundamental.xfepsttm <= 0.0 && xfundamental.xfepsfwd <= 0.0 )
	{
		ErrorCount++;
		return ( 0 );
	}

	if (  xfundamental.xfgrow5 <= 0.0 )
	{
		ErrorCount++;
		return ( 0 );
	}

	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/value_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

	GrahamValue = IntrinsicValue ( GRAHAM_METHOD );
	GrahamFlag = CalcBuyHoldSell ( GrahamValue );
	ModernValue = IntrinsicValue ( MODERN_METHOD );
	ModernFlag = CalcBuyHoldSell ( ModernValue );

	fprintf ( fpOutput, "%s|", xstock.xsticker );
	fprintf ( fpOutput, "%s|", xstock.xsname );
	fprintf ( fpOutput, "%.2f|", xstock.xsclose );
	fprintf ( fpOutput, "%.2f|", GrahamValue );
	fprintf ( fpOutput, "%s|", GrahamFlag );
	fprintf ( fpOutput, "%.2f|", ModernValue );
	fprintf ( fpOutput, "%s|", ModernFlag );

	if ( xfundamental.xfreco < 1.0 )
	{
		fprintf ( fpOutput, " |" );
	}
	else if ( xfundamental.xfreco >= 1.0 && xfundamental.xfreco < 1.5 )
	{
		fprintf ( fpOutput, "STRONG BUY|" );	// 1
	}
	else if ( xfundamental.xfreco >= 1.5 && xfundamental.xfreco < 2.5 )
	{
		fprintf ( fpOutput, "BUY|" );			// 2
	}
	else if ( xfundamental.xfreco >= 2.5 && xfundamental.xfreco < 3.5 )
	{
		fprintf ( fpOutput, "HOLD|" );			// 3
	}
	else if ( xfundamental.xfreco >= 3.5 && xfundamental.xfreco < 4.5 )
	{
		fprintf ( fpOutput, "SELL|" );			// 4
	}
	else if ( xfundamental.xfreco >= 4.5 )
	{
		fprintf ( fpOutput, "STRONG SELL|" );	// 5
	}

	fprintf ( fpOutput, "\n" );

	return ( 0 );
}
