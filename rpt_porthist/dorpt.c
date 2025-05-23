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

#include	"rpt_porthist.h"

#define	LIMIT_NUMBER_PRINTED
#undef	LIMIT_NUMBER_PRINTED

static	COLUMN_HEADINGS	ColumnArray[] = 
{
	{ "DATE",	"",		INIT_STRING_LEFT },
	{ "INVEST",	"",		INIT_DOUBLE_RIGHT },
	{ "CASH",	"",		INIT_DOUBLE_RIGHT },
	{ "BONDS",	"",		INIT_DOUBLE_RIGHT },
	{ "EQUITY",	"",		INIT_DOUBLE_RIGHT },
	{ "TOTAL",	"",		INIT_DOUBLE_RIGHT },
	{ "ROI",	"",		INIT_DOUBLE_RIGHT },
	{ "CASH ALLOC",		"",		INIT_DOUBLE_RIGHT },
	{ "BOND ALLOC",		"",		INIT_DOUBLE_RIGHT },
	{ "EQUITY ALLOC",	"",		INIT_DOUBLE_RIGHT },
	{ "HIGH",	"",		INIT_DOUBLE_RIGHT },
	{ "PCT",	"",		INIT_DOUBLE_RIGHT },
};

static	int				ColumnCount = sizeof(ColumnArray) / sizeof(COLUMN_HEADINGS);

void dorpt ( char *DataFilename )
{
	char	xbuffer[1024];
#define		MAXTOKS		20
	char	*tokens[MAXTOKS];
	int		tokcnt;
	FILE	*fpData;
	double	HighValue;
	int		lineno;

	ReportOptions.HeaderRows = 1;
	ReportOptions.Title = "PORTFOLIO PERFORMANCE REPORT";
	ReportOptions.Subtitle = "";
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
			ReportOptions.LinesPerPage = 42;
			ReportOptions.WritePageNumbers = 1;
			break;
	}
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "porthist", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	fpData = rptinit ( DataFilename, &ReportOptions, ColumnArray, ColumnCount );

	lineno = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		lineno++;

		tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		if ( lineno == 1 )
		{
			HighValue = atof(tokens[10]);
		}
#ifdef LIMIT_NUMBER_PRINTED
		else if ( lineno > 100 || HighValue > atof(tokens[10]) )
		{
			break;
		}
#endif

		rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, imin(tokcnt,ColumnCount) );

#ifndef LIMIT_NUMBER_PRINTED
		if ( HighValue > atof(tokens[10]) )
		{
			break;
		}
#endif
	}

	rptclose ( &ReportOptions );
}
