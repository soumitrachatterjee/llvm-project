#include<stdio.h>

// Define an enumeration
enum Day {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};
enum Month{
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};
int main() {
    printf("Symbolic Name of %d : %s \n",Day::Thursday,__nameof(Day::Thursday));
    printf("Symbolic Name of %d : %s \n",Month::February, __nameof(Month::February));
    return 0;
}

