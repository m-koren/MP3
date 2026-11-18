//Author: Mason Koren
//Date: 11/11/2026
//File: MP3.cpp
//Description: Write a general-purpose program that you can run after any trip to calulate total costs of gas during each trip. Showing details like max and min trip cost, or converting from Liters to Gallons.

#include <iostream>
#include <cstdio>

using namespace std;

const int MAX_FILLUPS = 20;

double toGallons(double amount, char unit);
double fillUpCost(double amount, double pricePerUnit);
double milesPerGallon(double miles, double gallons);
double costPerMile(double cost, double miles);
double sumOf(double values[], int count);
int indexOfSmallest(double values[], int count);
int indexOfLargest(double values[], int count);

int main()
{
    //LOG ARRAYS: one per column, same index = same fill-up
    double odoReadings[MAX_FILLUPS + 1];  // slot 0 = start, slot 1 = fill-up 1, ...
    double pumped[MAX_FILLUPS];           // amount pumped, in the pump's unit
    char unitLetters[MAX_FILLUPS];        // 'G' or 'L'
    double unitPrice[MAX_FILLUPS];        // price per unit on the pump
    double pumpTotal[MAX_FILLUPS];        // total sale on the pump

    //LEG ARRAYS: computed after all input is read
    double legMiles[MAX_FILLUPS];
    double legGallons[MAX_FILLUPS];
    double legCost[MAX_FILLUPS];
    double legCostPerMile[MAX_FILLUPS];

    //INPUT
    printf("Enter starting odometer reading: ");
    cin >> odoReadings[0];

    int numFillUps;
    printf("Enter number of fill-ups: ");
    cin >> numFillUps;
    while (numFillUps < 1 || numFillUps > MAX_FILLUPS)
    {
        printf("Error: enter a number from 1 to %d.\n", MAX_FILLUPS);
        printf("Enter number of fill-ups: ");
        cin >> numFillUps;
    }

    for (int stop = 0; stop < numFillUps; stop++)
    {
        bool lineIsGood = false;
        while (!lineIsGood)
        {
            printf("Fill-up %d (odometer, amount, G or L, price, total): ", stop + 1);
            cin >> odoReadings[stop + 1] >> pumped[stop] >> unitLetters[stop]
                >> unitPrice[stop] >> pumpTotal[stop];

            if (unitLetters[stop] != 'G' && unitLetters[stop] != 'L')
                printf("Error: unit must be G or L.\n");
            else if (odoReadings[stop + 1] <= odoReadings[stop])
                printf("Error: odometer must be greater than the last reading.\n");
            else
                lineIsGood = true;
        }
    }

    double tripBudget;
    printf("Enter fuel budget for the trip: ");
    cin >> tripBudget;

    //COMPUTE EACH LEG
    for (int leg = 0; leg < numFillUps; leg++)
    {
        legMiles[leg] = odoReadings[leg + 1] - odoReadings[leg];
        legGallons[leg] = toGallons(pumped[leg], unitLetters[leg]);
        legCost[leg] = pumpTotal[leg];
        legCostPerMile[leg] = costPerMile(legCost[leg], legMiles[leg]);
    }

    //TRIP TOTALS
    double tripMiles = sumOf(legMiles, numFillUps);
    double tripGallons = sumOf(legGallons, numFillUps);
    double tripCost = sumOf(legCost, numFillUps);

    //REPORT
    printf("\nLeg   Miles  Gallons   MPG      Cost  Cost/mile\n");
    for (int leg = 0; leg < numFillUps; leg++)
    {
        printf("%3d %7.1f %8.2f %5.1f $%8.2f $%8.3f\n",
            leg + 1, legMiles[leg], legGallons[leg],
            milesPerGallon(legMiles[leg], legGallons[leg]),
            legCost[leg], legCostPerMile[leg]);
    }

    printf("\nTotal miles: %.1f\n", tripMiles);
    printf("Total gallons: %.2f\n", tripGallons);
    printf("Overall MPG (miles per gallon): %.1f\n", milesPerGallon(tripMiles, tripGallons));
    printf("Total fuel cost: $%.2f\n", tripCost);
    printf("Overall cost per mile: $%.3f\n", costPerMile(tripCost, tripMiles));

    printf("Cheapest leg per mile: Leg %d\n", indexOfSmallest(legCostPerMile, numFillUps) + 1);
    printf("Most expensive leg per mile: Leg %d\n", indexOfLargest(legCostPerMile, numFillUps) + 1);

    if (tripCost < tripBudget)
        printf("You were under budget by $%.2f\n", tripBudget - tripCost);
    else if (tripCost > tripBudget)
        printf("You were over budget by $%.2f\n", tripCost - tripBudget);
    else
        printf("You were exactly on budget.\n"); //FIX THIS LINE, IT DOESN'T PRINT RIGHT WHEN AT 0.00

    return 0;
}

double toGallons(double amount, char unit)
{
    if (unit == 'L') //FIX THIS CONVERSION, CANADIAN VS US $
        return amount / 3.785;
    return amount;
}

double fillUpCost(double amount, double pricePerUnit)
{
    return amount * pricePerUnit;
}

double milesPerGallon(double miles, double gallons)
{
    return miles / gallons;
}

double costPerMile(double cost, double miles)
{
    return cost / miles;
}

double sumOf(double values[], int count)
{
    double sum = 0;
    for (int i = 0; i < count; i++)
        sum += values[i];
    return sum;
}

int indexOfSmallest(double values[], int count)
{
    int best = 0;
    for (int i = 1; i < count; i++)
        if (values[i] < values[best])
            best = i;
    return best;
}

int indexOfLargest(double values[], int count)
{
    int best = 0;
    for (int i = 1; i < count; i++)
        if (values[i] > values[best])
            best = i;
    return best;
}