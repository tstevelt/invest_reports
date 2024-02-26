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

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<ctype.h>

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <sys/types.h>

#include	"shslib.h"

#include	"dbylib.h"
#include	"rptlib.h"

#define		MEMBER
#define		STOCK
#define		SECTOR
#define		INDUSTRY
#define		AVERAGE
#define		WATCHLIST
#define		PORTFOLIO
#define		TEMP
#include	"fileinvest.h"
#include	"invlib.h"

#define		SELECT_TOP			201
#define		SELECT_BOTTOM		202
#define		SELECT_MINIMUM		203
#define		SELECT_MAXIMUM		204

TYPE	long				ReportMember;
TYPE	REPORT_OPTIONS		ReportOptions;
TYPE	int					Debug;
TYPE	char				StockIndex;	
TYPE	int					SelectStyle;
TYPE	int					ReportCount;
TYPE	double				MinimumRS;
TYPE	double				MaximumRS;

TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;

/*----------------------------------------------------------
	mysql and dbylib stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
TYPE	char		Statement[1024];
TYPE	char		WhereClause[1024];
TYPE	char		OrderByClause[1024];
#ifdef MAIN
char	*LogFileName = "/var/local/invest.log";
#else
TYPE	char	*LogFileName;
#endif

/*------------------------------
:.,$d
:r ! mkproto -p *.c
------------------------------*/


/* dorpt.c */
void dorpt ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* rs.c */
int main ( int argc , char *argv []);

/* sortdata.c */
void sortdata ( void );
