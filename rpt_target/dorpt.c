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

void dorpt ( char *DataFilename )
{
	char	xbuffer[1024];
#define		MAXTOKS		100
	FILE	*fpData;
	int		lineno;

	ReportOptions.HeaderRows = 1;
	ReportOptions.Title = "ALLOCATION BY ASSET TYPE";
	ReportOptions.Subtitle = "";
	ReportOptions.BreakCount = 0;
	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_HTML:
			ReportOptions.LinesPerPage = 0;
			ReportOptions.WritePageNumbers = 0;
			break;
		case RPT_FORMAT_TEXT:
			ReportOptions.LinesPerPage = 42;
			ReportOptions.WritePageNumbers = 1;
			break;
	}
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "allocation", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	if (( fpData = fopen ( DataFilename, "r" )) == NULL )
	{
		printf ( "Cannot open %s\n", DataFilename );
		exit ( 1 );
	}

	if (( fpOutput = fopen ( ReportOptions.OutputFilename, "w" )) == NULL )
	{
		printf ( "Cannot create %s\n", ReportOptions.OutputFilename );
		exit ( 1 );
	}

	lineno = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		lineno++;

		if ( lineno == 1 && ReportOptions.Format == RPT_FORMAT_HTML )
		{
			fprintf ( fpOutput, "<pre>\n" );
		}
		fprintf ( fpOutput, "%s", xbuffer );
	}
	fprintf ( fpOutput, "</pre>\n" );

	fclose ( fpData );
	fclose ( fpOutput );
}
