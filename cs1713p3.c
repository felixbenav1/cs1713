/***********************************************************************************
cs1713p3.c by Felix Benavides   (skeletal version)
Purpose:
    This program reads flight information and a command file.   It 
    processes the commands against the flight information.
    This file contains the functions that students write.
Command Parameters:
    p3 -f flightFileName -c commandFileName
Input:
    Flight   Stream input file which contains many records defining flights:
                 szFlightId szFrom  szDest  szDepartTm  iAvailSeats dSeatPrice 
                 10s        3s      3s      5s          4d          10lf 

    Command  This is different from the previous assignment.  The file contains 
             text in the form of commands (one command per text line):  
                 CUSTOMER BEGIN cGender   szBirthDt   szEmailAddr    szFullName
                     specifies the beginning of customer request and includes 
                     all the identification information from program 2.
                 CUSTOMER ADDRESS szStreetAddress,szCity,szStateCd,szZipCd
                     specifies the address for a customer (separated by commas)
                 CUSTOMER REQUEST szFlightId iNumSeats
                     specifies a single flight request.  Steps:
                     >	Print the flight ID and requested number of seats
                     >	Lookup the flight ID using a binary search.  If not found,
                        print a warning (but do not terminate your program) and return.
                     >	If found, try to satisfy the entire requested number of seats.
                        If not enough seats,  print a warning and return.
                     >	Print the unit price and cost.
                     >	Accumulate the total cost for this customer
                 CUSTOMER COMPLETE
                     specifies the completion of the list of flight requests 
                     for a customer.
                 FLIGHT INCREASE szFlightId iQuantity
                     increase the available seats for a flight by the specified quantity.
                 FLIGHT SHOW szFlightId    
                     requests a display of a particular flight.  Show all of its information.

Results:
    Prints the Flights prior to sorting
    Prints the Flight after sorting.
    Processes the commands (see above) and shows any errors.
    Prints the resulting Flights
Returns:
    0  normal
    -1 invalid command line syntax
    -2 show usage only
    -3 error during processing, see stderr for more information

Notes:
    p3 -?       will provide the usage information.  In some shells,
                you will have to type p3 -\?

**********************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cs1713p3.h"

/******************** getFlights **************************************
    int getFlights(Flight flightM[])
Purpose: 
    Populates the flightM array with the information of the available
    flights and functionally returns the amount of available flights.

Parameters:
    I Flight flightM[]  Array that needs to be populated
    O iFlightCnt        Number of flights.
Returns:
    Returns the number of flights in the array.
Notes:
    
**************************************************************************/
int getFlights(Flight flightM[], char * pszFlightFileName)
{
    char szInputBuffer[100];		// input buffer for reading data
    int i = 0;                      // subscript in flightM
    int iScanfCnt;                  // returned by sscanf
    FILE *pFileFlight;              // Stream Input for Flights data. 
    int iFlightCnt = 0;

    /* open the Flights stream data file */
    if (pszFlightFileName == NULL)
        exitError(ERR_MISSING_SWITCH, "-f");

    pFileFlight = fopen(pszFlightFileName, "r");
    if (pFileFlight == NULL)
        exitError(ERR_FLIGHT_FILENAME, pszFlightFileName);

    /* get flight information until EOF
    ** fgets returns null when EOF is reached.
    */

    /**** your code ******/
    for(i = 0; i < MAX_FLIGHTS; i++)
    {
        if(fgets(szInputBuffer, 100, pFileFlight) != NULL)
        {
            iScanfCnt = sscanf(szInputBuffer, "%10s %3s %3s %5s %d %lf[^\n]\n"
            , flightM[i].szFlightId
            , flightM[i].szFrom
            , flightM[i].szDest
            , flightM[i].szDepartTm
            , &flightM[i].iAvailSeatCnt
            , &flightM[i].dSeatPrice);

            //check flight information
            if(iScanfCnt < 6)
                exitError("Bad Flight information", "");
            //good flight info, increment flight count
            iFlightCnt++;
        }
    }
    //close file to prevent memory leaks
    fclose(pFileFlight);
    return iFlightCnt;

}
/******************** sortFlights **************************************
    void sortFlights(Flight flightM[], int iFlightCnt)
Purpose:
    Function sorts flights in the flightM array in alphabetical
    order using a bubble sort. Does not return any value.
Parameters:
    I Flight flightM[]  Array of flight information.
    I int iFlightCnt    Number of flights in the array.
Returns:
    None.
Notes:
    
**************************************************************************/
void sortFlights(Flight flightM[], int iFlightCnt)
{
    /**** your code ******/
    int i;
    int j;
    int bChange;
    Flight flightTemp;

    for(i = 0; i < (iFlightCnt - 1); i++)
    {
        bChange = FALSE;
        for(j = 0; j < (iFlightCnt - i - 1); j++)
        {
            //check if next flight is less than current
            if(strcmp(flightM[j+1].szFlightId, flightM[j].szFlightId) < 0)
            {
                //swap positions
                flightTemp = flightM[j+1];
                flightM[j+1] = flightM[j];
                flightM[j] = flightTemp;
                //update flag showing a change was made
                bChange = TRUE;

            }
        
        }
        //check if anything was changed in last iteration
        if(bChange == FALSE)
            break;
    }

}

/******************** printFlights **************************************
    void printFlights(char *pszHeading, Flight flightM[], int iFlightCnt)
Purpose:
    Function will print out the initial flight array, the sorted
    flight array, and then the resulting flights in a table format
    with column headings. Does not return a value.
Parameters:
    I char *pszHeading      Column heading.
    I Flight flightM[]      Array of flight information.
    I int iFlightCnt        Number of Flights in array.
Returns:
    None.
Notes:
    
**************************************************************************/
void printFlights(char *pszHeading, Flight flightM[], int iFlightCnt)
{
    int i;
    printf("%s\n", pszHeading);
    printf("    %-10s   %-4s %-4s %-6s %-6s  %-10s\n"
        , "Flight Id", "From", "Dest", "Depart", "Avail", "Unit Price");

    for (i = 0; i < iFlightCnt; i++)
    {
        printf("    %-10s   %-4s %-4s %-6s %5d   %10.2lf\n"
            , flightM[i].szFlightId
            , flightM[i].szFrom
            , flightM[i].szDest
            , flightM[i].szDepartTm
            , flightM[i].iAvailSeatCnt
            , flightM[i].dSeatPrice);
    }
}

/********************processCustomerCommand *****************************
    void processCustomerCommand(Flight flightM[], int iFlightCnt
     , char *pszSubCommand, char *pszRemainingInput
     , Customer *pCustomer, double *pdCustomerRequestTotalCost )
Purpose:
    Processes the subcommands associated with the CUSTOMER command:
                 CUSTOMER BEGIN cGender   szBirthDt   szEmailAddr    szFullName
                     specifies the beginning of customer request and includes 
                     all the identification information from program 2.
                 CUSTOMER ADDRESS szStreetAddress,szCity,szStateCd,szZipCd
                     specifies the address for a customer (separated by commas)
                 CUSTOMER REQUEST szFlightId iNumSeats
                     specifies a single flight request.  Steps:
                     >	Print the flight ID and requested number of seats
                     >	Lookup the flight ID using a binary search.  If 
                        not found, print a warning (but do not terminate your 
                        program) and return.
                     >	If found, try to satisfy the entire requested number 
                        of seats.  If not enough seats, print a warning and 
                        return.
                     >	Print the unit price and cost.
                     >	Accumulate the total cost for this customer

                 CUSTOMER COMPLETE
                     specifies the completion of the list of flight requests 
                     for a customer.
Parameters:
    I/O Flight flightM[]              Array of flights
    I   int   iFlightCnt              Number of elments in flightM[]
    I   char  *pszSubCommand          Should be BEGIN, ADDRESS, REQUEST or COMPLETE
    I   char  *pzRemainingInput       Points to the remaining characters in the input
                                      line (i.e., the characters that following the
                                      subcommand).
    I/O Customer *pCustomer           The BEGIN subcommand begins a new customer.  The 
                                      customer's Request Total Cost must be set to 0.
    I/O double   *pdCustomerRequestTotalCost     The customer total cost.  This changes depending
                                      on the subcommand:
                                          BEGIN - set to 0
                                          REQUEST - add the cost  (unless there is a warning) 
Notes:

**************************************************************************/
void processCustomerCommand(Flight flightM[], int iFlightCnt
    , char *pszSubCommand, char *pszRemainingInput
    , Customer *pCustomer, double *pdCustomerRequestTotalCost)
{
    int iScanfCnt;
    FlightRequest flightRequest;
    
    // Determine what to do based on the subCommand
    if (strcmp(pszSubCommand, "BEGIN") == 0)
    {
        // get the Customer Identification Information
        // your code
        iScanfCnt = sscanf(pszRemainingInput, "%1c %10s %50s %31[^\n]\n"
        , &pCustomer->cGender
        , pCustomer->szBirthDt
        , pCustomer->szEmailAddr
        , pCustomer->szFullName);

        if(iScanfCnt < 4)
            exitError(ERR_CUSTOMER_ID_DATA, pszRemainingInput);

        return;

    }
    else if (strcmp(pszSubCommand, "COMPLETE") == 0)
    {
        // print the customer's total cost
        // your code
        printf("\t\t\t\t\t\t\t Total Cost    %.2lf\n"
        , *pdCustomerRequestTotalCost);
        printf("************************************************************\n");
        //make total cost zero for next customer
        *pdCustomerRequestTotalCost = 0.00;
        return;

    }
    else if (strcmp(pszSubCommand, "ADDRESS") == 0)
    {
        // get the postal address 
        // your code 
        iScanfCnt = sscanf(pszRemainingInput, "%30[^,],%21[^,],%2[^,],%5[^\n]\n]"
        , pCustomer->szStreetAddress
        , pCustomer->szCity
        , pCustomer->szStateCd
        , pCustomer->szZipCd);
        if(iScanfCnt < 4)
            exitError("Bad Customer Information", "");
    
        printf("%-15s %-15s %20s\n",
        "***************",
        "Flight Reservation Request",
        "***************");
        printf("%-10s %-10s (%1c %10s)\n%-35s\n%-10s, %2s %5s\n"
        , pCustomer->szEmailAddr
        , pCustomer->szFullName
        , pCustomer->cGender
        , pCustomer->szBirthDt
        , pCustomer->szStreetAddress
        , pCustomer->szCity
        , pCustomer->szStateCd
        , pCustomer->szZipCd);

        printf("\t\t\t\t%-10s %8s %10s %8s\n"
            , "Flight Id"
            , "Quantity"
            , "Unit Price"
            , "Cost");
        return;
    }
    else if (strcmp(pszSubCommand, "REQUEST") == 0)
    {
        int i;
        // get a flight request
        // your code 
        iScanfCnt = sscanf(pszRemainingInput, "%s %d[^\n]\n"
        , flightRequest.szFlightId
        , &flightRequest.iRequestSeats);
        if(iScanfCnt < 2)
            exitError("Invalid Flight Request Information", "");
    
        printf("\t\t\t\t%-10s %8d "
        , flightRequest.szFlightId
        , flightRequest.iRequestSeats);

        // find the flight in the array
        i = search(flightM, iFlightCnt, flightRequest.szFlightId);

        // your code
        //check if flight was found in array
        if(i == -1)
        {
            printf("*** flight not found ***\n");
            return;
        }
        //check if there are enough seats available for requested flight
        else if(flightRequest.iRequestSeats > flightM[i].iAvailSeatCnt)
        {
            printf("*** insufficient seats ***\n");
            return;
        }
        //else print details of request
        else
        {
            printf("%10.2lf %12.2lf\n"
            , flightM[i].dSeatPrice
            , flightM[i].dSeatPrice * flightRequest.iRequestSeats);

            //decrease number of total seats for requested flight
            flightM[i].iAvailSeatCnt -= flightRequest.iRequestSeats;
            //update the customer total cost by current requested flight
            *pdCustomerRequestTotalCost += flightM[i].dSeatPrice * flightRequest.iRequestSeats;
            return;
        }
  
    }
    else printf("   *** %s %s\n", ERR_CUSTOMER_SUB_COMMAND, pszSubCommand);
}
/********************processFlightCommand *****************************
    void processFlightCommand(Flight flightM[], int iFlightCnt
         , char *pszSubCommand, char *pszRemainingInput)
Purpose:
    Processes the subcommands associated with the FLIGHT command:
        FLIGHT INCREASE szFlightId iQuantity
               increase the available seats for a flight by the specified quantity.
        FLIGHT SHOW szFlightId    
               requests a display of a particular flight.  Show all of its information.
Parameters:
    I/O Flight flightM[]              Array of flights
    I   int   iFlightCnt              Number of elments in flightM[]
    I   char  *pszSubCommand          Should be RECEIVE or SHOW
    I   char  *pzRemainingInput       Points to the remaining characters in the input
                                      line (i.e., the characters that following the
                                      subcommand).
Notes:

**************************************************************************/
void processFlightCommand(Flight flightM[], int iFlightCnt
                             , char *pszSubCommand, char *pszRemainingInput)
{
    Flight flight;
    int iQuantity;      // quantity of seats 
    int iScanfCnt;
    int i;

    // Determine what to do based on the subCommand
    // your code
    if(strcmp(pszSubCommand, "SHOW") == 0)
    {
        iScanfCnt = sscanf(pszRemainingInput, "%10[^\n]\n"
        , flight.szFlightId);
        if(iScanfCnt < 1)
            exitError("Bad Flight Command", "");
        /*for(i = 0; i <= iFlightCnt, i++)
        {
            if(i == iFlightCnt)
                exitError("Flight not found", "");
            if(strcmp(flightM[i].szFlightId, flight.szFlightId) == 0)
            {
                printf("FLIGHT SHOW\n\t\t\t%10s %3s %3s %5s %d %.2lf"
                , flightM[i].szFlightId
                , flightM[i].szFrom
                , flightM[i].szDest
                , flightM[i].szDepartTm
                , flightM[i].iAvailSeatCnt
                , flightM[i].dSeatPrice);
                break;
             }   
        } */
        //find flight in array
        i = search(flightM, iFlightCnt, flight.szFlightId);
        //check if flight was found
        if(i == -1)
        {
            printf("\t\t\t *** flight not found ***\n\n");
            return;

        }
        //print flight requested to be shown
        else
        {
            printf("\t\t\t%-10s   %-4s %-4s %-6s %5d    %10.2lf\n\n"
            , flightM[i].szFlightId
            , flightM[i].szFrom
            , flightM[i].szDest
            , flightM[i].szDepartTm
            , flightM[i].iAvailSeatCnt
            , flightM[i].dSeatPrice);
            return;

        }

    }
    if(strcmp(pszSubCommand, "INCREASE") == 0)
    {
        iScanfCnt = sscanf(pszRemainingInput, "%10s %d[^\n]\n"
        , flight.szFlightId
        , &iQuantity);
        if(iScanfCnt < 2)
            exitError("Bad Flight Command", "");
        /*for(i = 0; i <= iFlightCnt; i++)
        {
            if(i == iFlightCnt)
                exitError("Flight not Found", "");
            if(strcmp(flightM[i].szFlightId, flight.szFlightId) == 0)
            {
                flightM[i].iAvailSeatCnt += iQuantity;
                break;
            }

        }*/
        //look for flight in array
        i = search(flightM, iFlightCnt, flight.szFlightId);
        //check if flight was found
        if(i == -1)
        {
            printf("\t\t\t *** flight not found ***\n\n");
            return;
        }
        //else increase number of seats for flight by given quantity
        else
        {
            flightM[i].iAvailSeatCnt += iQuantity;
        }

    }
    
}
/******************** search *****************************
    int search(Flight flightM[], int iFlightCnt, char *pszMatchFlightId)
Purpose:
    Function finds the wanted flight Id in the flight array using
    a binary search. Returns subscript of where the Flight ID match
    was found in the flight array.
Parameters:
    I   Flight flightM[]               Array of flights
    I   int   iFlightCnt               Number of elments in flightM[]
    I   char *pszMatchFlightId         Flight Id to find in the array
Returns:
    >= 0   subscript of where the match value was found
    -1     not found
Notes:

**************************************************************************/
int search(Flight flightM[], int iFlightCnt, char *pszMatchFlightId)
{
    // your code
    int iLowerBnd = 0;                  //variable for lowest subscript
    int iUpperBnd = iFlightCnt - 1;     //variable for highest subscript
    int iMidPnt;                        //variable for array midpoint
    while(iLowerBnd <= iUpperBnd)
    {
        //find midpoint between current upper and lower bounds
        iMidPnt = (iLowerBnd + iUpperBnd)/2;
        if(strcmp(flightM[iMidPnt].szFlightId, pszMatchFlightId) == 0)
            return iMidPnt;
        if(strcmp(flightM[iMidPnt].szFlightId, pszMatchFlightId) > 0)
            //wanted FlightID is less so new upper bound 
            //is 1 less than midpoint
            iUpperBnd = iMidPnt - 1;
        else
            //wanted FlightID is greater so new lower 
            //bound is 1 more than midpoint
            iLowerBnd = iMidPnt + 1;

    }
    //return -1 to show flight was not found in array
    return -1;

}
