#include <iostream>

// 1. Calculate the resistive voltage divider 1 to 3 (at the input voltage of 12V at the output should be 3V). 
// The maximum current through the divider should not exceed 5mA. Resistor values should be selected from the E24 series.


//4
double U_in = 12;
const int MAX_mA = 5;
int R_count = 4;

double R1 = 1.0;
double R2 = 1.0;
double R3 = 1.0;
double R4 = 1.0;

int main()
{
    double R_all = R1 + R2 + R3 + R4;
    double R_all_avarage = (R1 + R2 + R3 + R4) / R_count;
    double I = U_in / R_all; 

    
    if (I < MAX_mA)
    {
        double U_R1 = (U_in * R_all_avarage) / (R1 + R2 + R3 + R4); 
        std::cout << U_R1 << std::endl;

        double U_R2 = (U_in * R_all_avarage) / (R1 + R2 + R3 + R4);
        std::cout << U_R2 << std::endl;

        double U_R3 = (U_in * R_all_avarage) / (R1 + R2 + R3 + R4);
        std::cout << U_R3 << std::endl;
    }

}