#include <iostream>

extern "C" {
    double abcd(double,double,double,double);
}

int main() {
    std::cout << "abcd of 3.0 and 4.0 and 5.0 and 6.0: " << abcd(3.0, 4.0, 5.0, 6.0) << std::endl;
}
