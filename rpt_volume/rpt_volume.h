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
#include	<math.h>

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
#define		HISTORY
#define		AVERAGE
#define		WATCHLIST
#define		PORTFOLIO
#define		TEMP
#include	"fileinvest.h"
#include	"invlib.h"


/*----------------------------------------------------------
	app defines and variables
----------------------------------------------------------*/
#define		MODE_RWALK		'R'
#define		MODE_TOP		'T'
TYPE	int		RunMode;
TYPE	char	*StartDate;
TYPE	char	*EndDate;
TYPE	int		Portfolio;
TYPE	char	ReportType;
TYPE	char	StockIndex;
TYPE	char	*Ticker;
TYPE	int		Generations;
TYPE	char	*InputFileName;
TYPE	int		GenerationIndex;
TYPE	REPORT_OPTIONS	ReportOptions;
TYPE	long	ReportMember;
TYPE	int		Debug;

TYPE	double	TopPercent;
TYPE	int		MinimumVolume;
TYPE	int		NumberOfDays;
TYPE	int		NumberOfDaysPrevious;
TYPE	int		NumberOfDaysCurrent;


typedef struct
{
	char	xsticker[21];
	char	xsname[31];
	double	Intercept;
	double	Slope;
	double	CorrCoeff;
	double	StdDev;
	double	Return;
	double	Variance;
} STOCK_RECORD;

#define		MAXSTOCKS	3500
TYPE	STOCK_RECORD	StockArray[MAXSTOCKS];
TYPE	int				StockCount;
TYPE	int				MissingStartDate;
TYPE	int				MissingEndDate;
TYPE	int				MissingHighLowPrices;
TYPE	double			BuyTotal;
TYPE	double			SellTotal;
TYPE	int				Direction;
TYPE	double			ReturnPercent;

TYPE	int			ReportCount;
TYPE	FILE		*fpOutput;
TYPE	char		OutFileName[256];

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

/*----------------------------
:.,$d
:r ! mkproto -p *.c
----------------------------*/

/* dorpt_accumulate.c */
int dorpt_accumulate ( char *DataFilename );

/* dorpt_increase.c */
int dorpt_increase ( char *DataFilename );

/* EachStockAccumulate.c */
int EachStockAccumulate ( void );

/* EachStockIncrease.c */
int EachStockIncrease ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* sortdata.c */
void sortdata ( char *OutFileName , int FieldNumber );

/* volume.c */
int main ( int argc , char *argv []);
