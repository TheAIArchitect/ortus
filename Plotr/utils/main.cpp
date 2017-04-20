//#include "MatPlotLibCpp.hpp"

//namespace plt = matplotlibcpp;
#include "../Plotr.hpp"
#include <cmath>
#include <vector>

int main(int argc, char** argv){
    //plot::plot({1,2,3,4});
    //plot::show();
    //exit(0);
    //return 0;
    Plotr plt;
    
    // Prepare data.
    int n = 5000;
    std::vector<double> x(n), y(n), z(n), w(n,2);
    for(int i=0; i<n; ++i) {
        x.at(i) = i*i;
        y.at(i) = sin(2*M_PI*i/360.0);
        z.at(i) = log(i);
    }
    
    // Plot line from given x and y data. Color is selected automatically.
    plt.plot(x, y);
    // Plot a red dashed line from given x and y data.
    plt.plot(x, w,"r--");
    // Plot a line whose name will show up as "log(x)" in the legend.
    plt.named_plot("log(x)", x, z);
    
    // Set x-axis to interval [0,1000000]
    plt.xlim(0, 1000*1000);
    
    // Add graph title
    plt.title("Sample figure");
    // Enable legend.
    plt.legend();
    
    plt.show();
    // save figure
    //plt::save("./basic.png");
    
}
