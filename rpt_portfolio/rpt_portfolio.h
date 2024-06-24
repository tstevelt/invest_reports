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
#define		FUNDAMENTAL
#define		STOCK
#define		HISTORY
#define		AVERAGE
#define		PORTFOLIO
#define		DIVIDEND
#include	"fileinvest.h"
#include	"invlib.h"

#define		PRICEFILE		"/var/local/tmp/rpt_current.json"

TYPE	int		UseTiingo;
TYPE	int		Debug;
TYPE	int		ReportCount;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	long	ReportMember;
TYPE	int		ReportStyle;
TYPE	char	StockType;
TYPE	char	Broker[12];
TYPE	int		SortBy;
TYPE	DATEVAL	dvToday;
TYPE	DATEVAL	dvYearAgo;
TYPE	char	ascYearAgo[12];
TYPE	double	TotalInvest;
TYPE	double	TotalValue;

#define		MODE_REPORT		'R'
TYPE	int		RunMode;
TYPE	char	OneTicker[22];

TYPE	REPORT_OPTIONS	ReportOptions;

typedef struct
{
	char	Type;
	char	Ticker[20];
	DATEVAL	Date;
	double	Shares;
	double	Cost;
} MSTAR_RECORD;

TYPE	MSTAR_RECORD	MstarArray[PAID_MEMBER_PORTFOLIO_LIMIT];
TYPE	int				MstarCount;

/*----------------------------------------------------------
	mysql and dbylib stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
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



/* dorpt_alerts.c */
void dorpt_alerts ( char *DataFilename );

/* dorpt_base.c */
void dorpt_base ( char *DataFilename );

/* dorpt_mstar.c */
void dorpt_mstar ( char *DataFilename );

/* dorpt_fundamental.c */
void dorpt_fundamental ( char *DataFilename );

/* dorpt_today.c */
void dorpt_today ( char *DataFilename );

/* dorpt_yoy.c */
void dorpt_yoy ( char *DataFilename );

/* dorpt_overnight.c */
void dorpt_overnight ( char *DataFilename );

/* dorpt_full.c */
void dorpt_full ( char *DataFilename );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* gettotal.c */
int gettotal ( void );

/* getdata.c */
int EachHistory ( void );
int getdata ( void );

/* portfolio.c */
int main ( int argc , char *argv []);

/* sortdata.c */
void sortdata ( char *OutFileName );
