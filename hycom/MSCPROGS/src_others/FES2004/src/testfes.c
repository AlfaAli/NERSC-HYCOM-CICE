/*
 *  C program for the FES prediction software.
 * 
 *  File      : testfes.c
 *  Developer : CLS - CNRS/LEGOS
 *  Version   : 1.3
 *  Date      : 13 January 2005
 *  
 * ----------------------------------------------------------------------
 *
 *  rc      : return code (problem if rc != 0)
 *  lat     : latitude
 *  lon     : longitude
 *  time    : time in CNES Julian days
 *  hour    : hour
 *  tide    : short tides (semi_diurnal and diurnal tides)
 *  lp      : long period tides
 *  load    : loading effects for short tide
 *  loadlp  : loading effects for long period tides
              (is always equal to zero)
 * 
 *  tide+lp             = pure tide (as seen by a tide gauge)
 *  tide+lp+load        = geocentric tide ((as seen by a satellite)
 *  CNES Julian day     = NASA Julian day + 2922
 *  CNES Julian day 0 is at midnight between the 31 December 
 *                          and 01 January 1950 AD Gregorian
 *
 */       

#include <stdlib.h>
#include <stdio.h>

#include "fes.h"

int main(void)
{
  fesData*	shortTide = NULL;
  fesData*	radialTide = NULL;
  int		rc	= 0;
  double	tide;
  double	lp;
  double	load;
  double	loadlp;

  double	lon	= -7.688;
  double	lat	= 59.195;
  double	time	= 12053;	/* 1983-01-01 00:00:00.0 */

  int		hour	= 0;
  char* fespath;

  
  /* Initialize memory for FES algorithms */
  if ( getenv("FES_PATH") == NULL ) {
            printf("\nError: Set enironment variable FES_PATH to the location of FES data \n");
         return -1;
   } else {
     fespath=getenv("FES_PATH") ;
   }
  /*printf("PATH read : %s\n",fespath );*/

  rc = fesNew(&shortTide, FES_TIDE, FES_IO, fespath);
  if ( rc != FES_SUCCESS )
      goto onError;

  rc = fesNew(&radialTide, FES_RADIAL, FES_IO, fespath);
  if ( rc != FES_SUCCESS )
      goto onError;

  printf("%12s %5s %9s %9s %9s %9s %9s %9s %9s\n",
    "JulDay","Hour","Latitude","Longitude",
    "Short_tid","LP_tid","Pure_Tide","Geo_Tide","Rad_Tide");

  for(hour = 0; hour < 24; hour++)
  {
    /* Compute tide */
    rc = fesCore(shortTide, lat, lon, time, &tide, &lp);
    if ( rc == FES_SUCCESS )
    {
      /* Compute load tide */
      rc = fesCore(radialTide, lat, lon, time, &load, &loadlp);
      if(  rc != FES_NO_DATA && rc == FES_SUCCESS )
      {
         printf("%12.5f %5d %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f\n",
           time,
           hour, 
           lat,
           lon, 
           tide,
           lp,
           tide+lp,
           tide+lp+load,
           load);
      }
    }
    else
    {
      goto onError;
    }
    time += 1/24.0;
  }

  goto onTerminate;

onError:
  printf("#ERROR : %s\n", fesErr2String(rc)),
  rc = 1;

onTerminate:
  /* Free memory for FES algorithms */
  fesDelete(shortTide);
  fesDelete(radialTide);

  return rc;
}
