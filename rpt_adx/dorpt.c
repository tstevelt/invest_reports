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

#include	"rpt_adx.h"

static	COLUMN_HEADINGS	ColumnArray [] = 
{
	{ "TICKER",		"",			INIT_STRING_LEFT },
	{ "COMPANY",	"",			INIT_STRING_LEFT },
	{ "PRICE",		"",			INIT_DOUBLE_RIGHT },
	{ "SIGNAL",		"",			INIT_STRING_CENTER },
	{ "-DI",		"",			INIT_DOUBLE_RIGHT },
	{ "+DI",		"",			INIT_DOUBLE_RIGHT },
	{ "ADX",		"",			INIT_DOUBLE_RIGHT },
	{ "RS",			"",			INIT_DOUBLE_RIGHT },
};

static  int		ColumnCount = sizeof(ColumnArray) / sizeof(COLUMN_HEADINGS);

void dorpt ()
{
	char	xbuffer[1024];
#define		MAXTOKS		10
	char	*tokens[MAXTOKS];
	int		tokcnt;
	FILE	*fpData;

	ReportOptions.HeaderRows = 1;
	switch ( BuySellAll )
	{
		case 'B':
			ReportOptions.Title = "ADX TRENDING - BUY SIGNAL";
			break;
		case 'S':
			ReportOptions.Title = "ADX TRENDING - SELL SIGNAL";
			break;
		case 'A':
			ReportOptions.Title = "ADX TRENDING - BUYS AND SELLS";
			break;
	}
	ReportOptions.Subtitle = IndexName ( StockIndex );
	ReportOptions.BreakCount = 0;
	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_HTML:
			ReportOptions.LinesPerPage = 0;
			ReportOptions.WritePageNumbers = 0;
			break;
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			ReportOptions.LinesPerPage = 60;
			ReportOptions.WritePageNumbers = 1;
			break;
	}
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "adx", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	fpData = rptinit ( OutFileName, &ReportOptions, ColumnArray, ColumnCount );

	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		switch ( ReportOptions.Format )
		{
			case RPT_FORMAT_APPEND_W:
			case RPT_FORMAT_REPLACE_W:
			case RPT_FORMAT_TEMP:
				fprintf ( ReportOptions.fpOutput, "%s|\n", tokens[0] );
				break;
			default:
				rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, tokcnt );
				break;
		}
	}

	rpttotal ( &ReportOptions, ColumnArray, ColumnCount, 0 );

	rptclose ( &ReportOptions );
}
