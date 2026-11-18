//Author: Mason Koren
//Date: 11/11/2026
//File: MP3.cpp
//Description: Write a general-purpose program that you can run after any trip to calulate total costs of gas during each trip. Showing details like max and min trip cost, or converting from Liters to Gallons.

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

const int MAX_FILLUPS = 20;
const int DAYS_SIZE = 366;   // index = day of year, index 0 unused

double toGallons(double amount, char unit);
double fillUpCost(double amount, double pricePerUnit);
double milesPerGallon(double miles, double gallons);
double costPerMile(double cost, double miles);
double sumOf(double values[], int count);
int indexOfSmallest(double values[], int count);
int indexOfLargest(double values[], int count);
int dayOfYear(int month, int day);
int loadRates(string fileName, double rates[], int size);
void fillMissingRates(double rates[], int size);

int main()
{
    //RATES: one per day of the year, -1 means no rate
    double rates[DAYS_SIZE];
    if (loadRates("rates2026.csv", rates, DAYS_SIZE) == -1)
    {
        printf("Error: could not open rates2026.csv.\n");
        return 1;
    }
    fillMissingRates(rates, DAYS_SIZE);

    //LOG ARRAYS: one per column, same index = same fill-up
    double odoReadings[MAX_FILLUPS + 1];  // slot 0 = start, slot 1 = fill-up 1, ...
    double pumped[MAX_FILLUPS];           // amount pumped, in the pump's unit
    char unitLetters[MAX_FILLUPS];        // 'G' or 'L'
    double unitPrice[MAX_FILLUPS];        // price per unit on the pump
    double pumpTotal[MAX_FILLUPS];        // total sale on the pump
    int fillDay[MAX_FILLUPS];             // day of the year of each fill-up

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
            int month, day;
            printf("Fill-up %d (month, day, odometer, amount, G or L, price per unit, total sale): ",
                stop + 1);
            cin >> month >> day >> odoReadings[stop + 1] >> pumped[stop]
                >> unitLetters[stop] >> unitPrice[stop] >> pumpTotal[stop];

            int d = dayOfYear(month, day);

            if (d == -1)
                printf("Error: that is not a real 2026 date.\n");
            else if (unitLetters[stop] != 'G' && unitLetters[stop] != 'L')
                printf("Error: unit must be G or L.\n");
            else if (unitLetters[stop] == 'L' && rates[d] < 0)
                printf("Error: no exchange rate is available for that date.\n");
            else if (odoReadings[stop + 1] <= odoReadings[stop])
                printf("Error: odometer must be greater than the last reading.\n");
            else
            {
                fillDay[stop] = d;
                lineIsGood = true;
            }
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

        //Canadian total sale -> U.S. dollars, rounded to the cent
        if (unitLetters[leg] == 'L')
            legCost[leg] = round(pumpTotal[leg] * rates[fillDay[leg]] * 100) / 100;
        else
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

    //Compare in whole cents so tiny floating-point differences don't matter
    long costCents = lround(tripCost * 100);
    long budgetCents = lround(tripBudget * 100);

    if (costCents < budgetCents)
        printf("You were under budget by $%.2f\n", (budgetCents - costCents) / 100.0);
    else if (costCents > budgetCents)
        printf("You were over budget by $%.2f\n", (costCents - budgetCents) / 100.0);
    else
        printf("You were exactly on budget.\n");

    return 0;
}

int dayOfYear(int month, int day)
{
    int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month < 1 || month > 12)
        return -1;
    if (day < 1 || day > daysInMonth[month - 1])
        return -1;

    int total = day;
    for (int m = 0; m < month - 1; m++)
        total += daysInMonth[m];
    return total;
}

int loadRates(string fileName, double rates[], int size)
{
    for (int i = 0; i < size; i++)
        rates[i] = -1;

    ifstream file(fileName);
    if (!file)
        return -1;

    string header;
    getline(file, header);   // skip the header line

    int month, day, count = 0;
    double rate;
    char comma;
    while (file >> month >> comma >> day >> comma >> rate)
    {
        int idx = dayOfYear(month, day);
        if (idx != -1 && idx < size)
        {
            rates[idx] = rate;
            count++;
        }
    }
    file.close();
    return count;
}

void fillMissingRates(double rates[], int size)
{
    int first = -1, last = -1;
    for (int i = 1; i < size; i++)
    {
        if (rates[i] != -1)
        {
            if (first == -1)
                first = i;
            last = i;
        }
    }
    if (first == -1)
        return;

    for (int i = first + 1; i <= last; i++)
        if (rates[i] == -1)
            rates[i] = rates[i - 1];
}

double toGallons(double amount, char unit)
{
    if (unit == 'L')
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