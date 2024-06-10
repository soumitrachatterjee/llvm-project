// RUN: %clangxx -std=c++11 -fsyntax-only -Xclang -verify %s 
#include <iostream>
using namespace std;
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

enum Month {
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
    // Check valid usage
    cout<<"Symbolic Name: %s\n"<<__nameof(Month::February); // expected-output {{Symbolic Name: Month::February}}
    cout<<"Symbolic Name: %s\n"<<__nameof(Day::Thursday); // expected-output {{Symbolic Name: Month::Thursday}}
    cout<<"Symbolic Name: %s\n"<<__nameof(Thursday); // expected-output {{Symbolic Name: Month::Thursday}}
    // Check invalid usage
    cout<<"Symbolic Name: %s\n"<<__nameof(1);  // expected-error {{Unsupported declaration type. Only enum constants are supported.}}
    cout<<"Symbolic Name: %s\n"<<__nameof("");  // expected-error {{Unsupported declaration type. Only enum constants are supported.}}
    return 0;
}