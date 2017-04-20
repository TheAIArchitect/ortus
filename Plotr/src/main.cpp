#include "Plotr.hpp"
#include "Plot.hpp"
#include <cmath>
#include <vector>

int main(int argc, char** argv){
    //plot::plot({1,2,3,4});
    //plot::show();
    //exit(0);
    //return 0;
    //Plotr plt;
    
    // Prepare data.
    int n = 1000;
    std::vector<double> x(n), y(n), z(n), yy(n), zz(n), w(n,2);
    for(int i=1; i<n; ++i) {
        x.at(i) = i;
        y.at(i) = sin(2*M_PI*i/360.0);
        yy.at(i) = sin(2*M_PI*i/360.0);
        z.at(i) = log(i);
        zz.at(i) = 7*i*i*i*i + 3*i*i*i + 2*i*i + 2*i;
        printf("x (0), y (0), z (1), yy (2), zz (3): %.f, %.f, %.f, %.f, %.f\n",x[i], y[i], z[i], yy[i], zz[i]);
    }
    Plot p(x, y);
    p.addYValues<double>(z);
    p.addYValues<double>(yy);
    p.addYValues<double>(zz);
    p.ion();
    
    std::unordered_map<std::string, std::string> things;
    things["label"] = "sin(x)";
    things["color"] = p.COLORS[0];
    //things["axes"] = p.COLORS[0];
    //things["marker"] = p.MARKERS[0];
    things["linestyle"] = p.STYLES[0];
    //things["title"] = p.COLORS[0];
    p.figure();
    p.subplot("211");
    p.title("This is a Sample Title for Subplot 1");
    p.plot(things,false, 0,0);
    //p.legend();
    things["linestyle"] = p.STYLES[1];
    things["color"] = p.COLORS[2];
    //things["marker"] = p.MARKERS[1];
    things["label"] = "log(x)";
    //p.figure(2);
    p.grid(true);
    p.subplot("212");
    p.plot(things,false, 0,1);
    //plt2.plot(x, y, labelVec);
    //labelVec["label"] = "log(x)";
    //plt2.plot(x, z, labelVec);
    p.title("This is a Sample Title for Subplot 2");
    // must call show if ion() not called
    p.legend();
    //p.show();
    p.draw();
    
    p.figure();
    things.clear();
    things["maxlags"] = "20";
    //things["usevlines"] = "False";
    p.xcorr(things,1,1);
    //p.plot("w");
    //p.plot(things);
    
    
    //  must call pause, with a time value, to get plot to display
    // if ion() has been called (it causes the plot to be non-blocking),
    // so, unless there is something else preventing holding things up,
    // after pause, eventually the interpreter will close the plot.
    p.pause(50);
   
    //std::string wtf;
    //std::cin >> wtf;
    
    
    exit(0);
    
    /*
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
    */
//    std::vector<int> pieStuff {4, 8, 12};
    /*
    std::vector<double> pieStuff(3);
    pieStuff.at(0) = 4.f;
    pieStuff.at(1) = 8.f;
    pieStuff.at(2) = 12.f;
    std::map<std::string, std::string> labelVec;
    labelVec["label"] = "something";
    
    //plt.plot(x, y);
    plt.pie(pieStuff, labelVec); // at this point, the second argument isn't being used... see function in Plotr.hpp
    
    plt.show();
    */
    // save figure
    //plt::save("./basic.png");
    
    /*
    Plotr plt2;
    std::map<std::string, std::string> labelVec;
    labelVec["label"] = "sin(x)";
    plt2.plot(x, y, labelVec);
    labelVec["label"] = "log(x)";
    plt2.plot(x, z, labelVec);
    plt2.legend();
    plt2.show();
     */
    
    /*
    Plotr plt3;
    plt3.plot(x, y, labelVec);
    plt3.plot(x, z, labelVec);
    plt3.legend();
    plt3.show();
    */
     
    return 0;
}
