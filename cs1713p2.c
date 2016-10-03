/**********************************************************************
cs1713p2.c by Felix Benavides
Purpose:
    This program reads customer flight reservations to produce output 
    suitable as a Reservation Request. 
Command Line Arguments:
    p2 -f flightFileName -c customerReservationFileName  
Input:
Flight  Stream input file which contains many records defining flights:
            szFlightId  szFrom  szDest  szDepartTm  IAvailSeats dSeatPrice
            10s         3s      3s      5s          4d          10lf
Customer  Stream input file which contains many customer reservations, each 
       containing possibly many flight requests.  There are three different 
          kinds of lines of data for each reservation:
        - Customer Identification Information:
            o One line per reservation request (separated by spaces)
              cGender   szBirthDt   szEmailAddr    szFullName 
              1c        10s         40s            30s (may contain spaces)
        - Customer Address Information 
            o One line per reservation request (separated by commas)
              szStreetAddress             szCity  szStateCd  szZipCd
              30s (may contain spaces)   20s     2s         5s
        - Flight Request:
            o 0 to many of them per reservation (terminated by END in the Flight ID)
              szFlightId    iNumSeats 
              10s           4d
Results:
    Prints Flight info and each Customer Reservation in a readable format.
    Examples:
Flight Id   From Destination Departure Time  Available Seats  Price per Seat
H100.15005  SAT     HOU         08:00               4           65.00
H100.15006  SAT     HOU         08:00               12          65.00
H111.15009  SAT     HOU         17:00               10          65.00
H200.15010  ATL     HOU         08:00               20          120:00

        ******************* Flight Reservation Request  **********************
        petem@xyz.net Pete Moss (M 1986/01/01)
        123 Boggy Lane
        New Orleans, LA 70112
           Flight       Seats      
           H100.15005      2      
           H222.15005      2
           H200.15010      2
           H333.15010      2       
        ******************* Flight Reservation Request  **********************
        pcorn@abc.net Pop Corn (M 1957/02/02)
        456 Kernel
        San Antonio, TX 78210
           Flight       Seats      
           H222.15005      1
           HXXX.XXXXX      1
           H333.15010      1       
Returns:
    0  normal
    -1 invalid command line syntax
    -2 show usage only
    -3 error during processing, see stderr for more information
Notes:
    reserve -?  will provide the usage information.  In some shells,
                you will have to type reserve -\?
  
**********************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cs1713p2.h"
FILE *pFileCust;               // stream Input for Customer Reservation data
FILE *pFileFlight;             // stream input for Flight Definitions

void processCommandSwitches(int argc, char *argv[]
      , char **ppszCustomerFileName
      , char **ppszFlightFileName);

void processReservations(Flight flightM[], int iFlightCnt);

int getFlights(Flight flightM[]);

void printFlights(Flight flightM[], int iFlightCnt);

int main(int argc, char *argv[])
{
    char *pszCustomerFileName = NULL;
    Flight flightM[MAX_FLIGHTS];
    int iFlightCnt;
    char *pszFlightFileName = NULL;

    processCommandSwitches(argc, argv
        , &pszCustomerFileName
        , &pszFlightFileName);
    
    if (pszFlightFileName == NULL)
        exitError(ERR_MISSING_SWITCH, "-f");

    pFileFlight = fopen(pszFlightFileName, "r");
    if (pFileFlight == NULL)
        exitError(ERR_FLIGHT_FILENAME, pszFlightFileName);

    iFlightCnt = getFlights(flightM);

    printFlights(flightM, iFlightCnt);
    // Process the command switches
    processCommandSwitches(argc, argv
        , &pszCustomerFileName
        , &pszFlightFileName);
    
    // open the Customer Reservation stream data file
    if (pszCustomerFileName == NULL)
        exitError(ERR_MISSING_SWITCH, "-c");
    
    pFileCust = fopen(pszCustomerFileName, "r");
    if (pFileCust == NULL)
        exitError(ERR_CUSTOMER_RES_FILENAME, pszCustomerFileName);
   
    // process the Reservations
    processReservations(flightM, iFlightCnt);
    
    printFlights(flightM, iFlightCnt);

    fclose(pFileFlight);
    fclose(pFileCust);
    printf("\n");    // included so that you can put a breakpoint on this line
    return 0;
}

/****** you need to document and code this function *****/

/******************* GetFlights *************************
int getFlights(Flight flightM[])
Purpose:
    Populates the flightM array with the information of 
    the available flights and functionally returns the
    amount of available flights.
Parameters:
    I Flight flightM[]      Array that needs to be populated.
    O iFlightCnt            Number of flights.
Notes:

Return:
    Returns the number of flights in the array.
********************************************************/
int getFlights(Flight flightM[])
{
    int iFlightCnt = 0;
    char szInputBuffer[100];
    int iScanfCnt;
    int i;
    
    for (i = 0; i < MAX_FLIGHTS; i++)
    {
        if(fgets(szInputBuffer, 100, pFileFlight) != NULL)
        {
            iScanfCnt = sscanf(szInputBuffer, "%10s %3s %3s %5s %d %lf[\n]\n"
            , flightM[i].szFlightId
            , flightM[i].szFrom
            , flightM[i].szDest
            , flightM[i].szDepartTm
            , &flightM[i].iAvailSeatCnt
            , &flightM[i].dSeatPrice);
            if(iScanfCnt < 6)
                exitError("Bad Flight Information", "");
            //increment iFlightCnt because there is good flight info
            iFlightCnt++;
        }

    }
    //return number of flights in array
    return iFlightCnt;
        
}

/******************** ProcessReservations ***************
void processReservations(Flight flightM[], int iFlightCnt);
Purpose:
    Function processes customers personal information as well
    as their flight reservation requests. Does not return any value.
Parameters:
    I Flight flightM[]      Array of Flight information.
    I int iFlightCnt        Number of entries in Flight array.
Notes:

Return value:
    None.
********************************************************/
void processReservations(Flight flightM[], int iFlightCnt)
{
    //intialize array for input stream
    char szInputBuffer[100];
    int iscanfCnt;
    double dTotalPrice;         //variable for entire Customer Request 
    double dRequestTotal;       //variable for single Flight Request
    int i;

    Customer customer;
    FlightRequest request;

    while(fgets(szInputBuffer, 100, pFileCust) != NULL)
    {
        dTotalPrice = 0.00;
        dRequestTotal = 0.00;
        printf("%-15s %-15s %20s\n",
        "******************",
        "Flight Reservation Request",
        "******************");

        //get passenger's personal information
        iscanfCnt = sscanf(szInputBuffer, "%1c %10s %50s %31[^\n]\n"
        , &customer.cGender
        , customer.szBirthDt
        , customer.szEmailAddr
        , customer.szFullName); 

        //handle input errors
        if(iscanfCnt < 4)
        {
            exitError("Bad information", "");
        }
       if(fgets(szInputBuffer, 100, pFileCust) != NULL)
       {

            iscanfCnt = sscanf(szInputBuffer, "%30[^,],%21[^,],%2[^,],%5[^\n]\n"
            , customer.szStreetAddress
            , customer.szCity
            , customer.szStateCd
            , customer.szZipCd);
            //handle input error
            if(iscanfCnt < 4)
            {
                exitError("Bad information", "");
            }

            //print passenger's personal information
            printf("%-10s  %-10s (%1c %10s)\n"
            , customer.szEmailAddr
            , customer.szFullName
            , customer.cGender
            , customer.szBirthDt);
            printf("%-35s\n"
            , customer.szStreetAddress);

            //print passenger's location
            printf("%-10s, %2s %5s\n"
            , customer.szCity
            , customer.szStateCd
            , customer.szZipCd);
            printf("%-10s %15s\n"
            , "Flight"
            , "Seats"); 
            while(fgets(szInputBuffer, 100, pFileCust) != NULL)
            {
                //get passenger's flights and requests
                iscanfCnt = sscanf(szInputBuffer, "%11s %4d[^\n]\n"
                , request.szFlightId
                , &request.iRequestSeats);
                //handle input error
                if(iscanfCnt < 2)
                {
                    exitError("Bad information", "");
                    break;
                }
                //check for end of passenger's requests
                if(strncmp(szInputBuffer, "END", 3) == 0)   
                    break;               
                //print passenger's flights and requests
                
                printf("%-10s %8d\n"
                , request.szFlightId
                , request.iRequestSeats);
                for(i = 0; i <= iFlightCnt; i++)
                {
                    //check if none of the Flight Id's have matched
                    if(i == iFlightCnt)
                    {
                        printf("%s\n", "Flight not found.");
                        break;
                    }
                    if(strcmp(flightM[i].szFlightId, request.szFlightId)
                        == 0)
                    {
                        //check if there are enough seats
                        if(request.iRequestSeats > flightM[i].iAvailSeatCnt)
                        {
                            printf("%s\n","Insufficient Seats.");
                            break;
                        }
                        //subtract from Flight's available seats
                        flightM[i].iAvailSeatCnt -= request.iRequestSeats;
                        //get total price for single request
                        dRequestTotal = request.iRequestSeats * flightM[i].dSeatPrice;
                        //add it to total price for that customer
                        dTotalPrice += dRequestTotal;
                        //print price per requested seat
                        printf("%s %.2lf\n", "Price per seat is"
                        , flightM[i].dSeatPrice);
                        //print price for all requested seats
                        printf("%s %.2lf\n", "Price for requested seats is"
                            , dRequestTotal);
                        break;
                    }
                }
            
            }
            //customer request is done, print total cost of entire request
            printf("%s %.2lf\n\n", "Total cost for request is"
                , dTotalPrice);
        }
    }
}

/******************** PrintFlights ************************
void printFlights(Flight flightM[], int iFlightCnt)
Purpose:
    This function will print out flight reservations and the flight
    information in a table format with column headings.
Parameters:
    I Flight flightm[]          Array with Flight information.
    I int iFlightCnt            Number of flights.
Notes:


Return value:
    None
**********************************************************/
void printFlights(Flight flightM[], int iFlightCnt)
{
    int i;

    //print header
    printf("%-13s  %8s   %11s   %14s   %15s   %14s\n"
    , "Flight ID"
    , "From"
    , "Destination"
    , "Departure Time"
    , "Available Seats"
    , "Price per Seat");

    for(i = 0; i < iFlightCnt; i++)
    {
        printf("%10s   %4s %15s  %29s  %29d  %29.2lf\n"
        , flightM[i].szFlightId
        , flightM[i].szFrom
        , flightM[i].szDest
        , flightM[i].szDepartTm
        , flightM[i].iAvailSeatCnt
        , flightM[i].dSeatPrice);
    }

}

/******************** processCommandSwitches *****************************
void processCommandSwitches(int argc, char *argv[], char **ppszCustomerFileName)
Purpose:
    Checks the syntax of command line arguments and returns the filenames.
    If any switches are unknown, it exits with an error.
Parameters:
    I   int argc                        count of command line arguments
    I   char *argv[]                    array of command line arguments
    O   char **ppszCustomerFileName     customer reservation file name
Notes:
    If a -? switch is passed, the usage is printed and the program exits
    with USAGE_ONLY.
    If a syntax error is encountered (e.g., unknown switch), the program
    prints a message to stderr and exits with ERR_COMMAND_LINE_SYNTAX.
**************************************************************************/
void processCommandSwitches(int argc, char *argv[], char **ppszCustomerFileName, char **ppszFlightFileName)
{
    int i;
    
    for (i = 1; i < argc; i++)
    {
        // check for a switch
        if (argv[i][0] != '-')
            exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
        // determine which switch it is
        switch (argv[i][1])
        {
            case 'c':                   // Customer Reservation File Name
                if (++i >= argc)
                    exitUsage(i, ERR_MISSING_ARGUMENT, argv[i - 1]);
                else
                    *ppszCustomerFileName = argv[i];
                break;
            case 'f':
                if(++i >= argc)
                    exitUsage(i, ERR_MISSING_ARGUMENT, argv[i - 1]);
                else
                    *ppszFlightFileName = argv[i];
                break;
            case '?':
                exitUsage(USAGE_ONLY, "", "");
                break;
            default:
                exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
        }
    }
}

/******************** exitError *****************************
    void exitError(char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    Prints an error message and diagnostic to stderr.  Exits with
    ERROR_PROCESSING.
Parameters:
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/
void exitError(char *pszMessage, char *pszDiagnosticInfo)
{
    fprintf(stderr, "Error: %s %s\n"
        , pszMessage
        , pszDiagnosticInfo);
    exit(ERROR_PROCESSING);
}
/******************** exitUsage *****************************
    void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    If this is an argument error (iArg >= 0), it prints a formatted message
    showing which argument was in error, the specified message, and
    supplemental diagnostic information.  It also shows the usage. It exits
    with ERR_COMMAND_LINE_SYNTAX.

    If this is just asking for usage (iArg will be -1), the usage is shown.
    It exits with USAGE_ONLY.
Parameters:
    I int iArg                      command argument subscript
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
{
    if (iArg >= 0)
        fprintf(stderr, "Error: bad argument #%d.  %s %s\n"
            , iArg
            , pszMessage
            , pszDiagnosticInfo);
    fprintf(stderr, "p2 -f flightFileName -c customerReservationName\n");
    if (iArg >= 0)
        exit(-1);
    else
        exit(-2);
}
