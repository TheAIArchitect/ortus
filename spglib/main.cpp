/**
 * Author:  Sean Grimes
 * Created: 09/14/15
 * Edited:  06/08/15
 *
 * Test file that shows how to use the various functions and classes
 */

#include <iostream>
#include <string>
#include <vector>  
#include <iterator> 
#include <iomanip>
#include "include/Logger.hpp"
#include "include/StrUtils.hpp"
#include "include/NumUtils.hpp"
#include "include/ExeTimer.hpp"
#include "include/dbHelper.hpp"
#include "include/FileUtils.hpp"
#include "include/TSQueue.hpp"
#include <fstream>
#include "include/GeneralUtils.hpp"
#include "include/ThreadPool.hpp"


using namespace std;

int main() {
/********** CLASS TESTING FUNCTIONS **********/
    /*
    ThreadPool t_pool(100);
    std::vector<Logger> loggers;
    for(auto i = 0; i < 100; ++i){
        loggers.emplace_back(new Logger(false));
    }
    for(auto i = 0; i < 100; ++i){
        cout << "i: " << i << endl;
        t_pool.push([&]{loggers[i].fatal(i);});
    }
    
    Logger log___;
    */
    
    ExeTimer get;
    get.start_timer();
    vector<string> get_vec = FileUtils::readLineByLine("test_input.txt");
    get.stop_timer();
    
    cout << "Get took: " << get.get_exe_time_in_ms() << " ms.\n";
    
    
    ExeTimer full;
    full.start_timer();
    string full_str = FileUtils::readFullFile("test_input.txt");
    full.stop_timer();
    
    cout << "Full took: " << full.get_exe_time_in_ms() << " ms.\n";
   
    ExeTimer wc_t;
    wc_t.start_timer();
    size_t wc_r = FileUtils::lineCount("test_input.txt");
    wc_t.stop_timer();
    cout << "wc took: " << wc_t.get_exe_time_in_ms() << " ms.\n";
    cout << "wc: " << wc_r << endl;
    
    
/********** UTILITY FUNCTION TESTING ***********/

// ----- Testing numOccuranceInVec ---------------------------------------------
    cout << "\n----- Testing numOccuranceInVec -----\n";
    vector<string> vec_1(10);
    string x = "a";
    for(int i = 0; i < vec_1.size(); ++i){
        vec_1[i] = x;
    }
    string z = "a";
    size_t e = GeneralUtils::numOccuranceInVec(vec_1, z);
    cout << "e: " << e << endl;
    
// ----- Testing trim ----------------------------------------------------------
    cout << "\n----- Testing trim -----\n";
    string s = "   \t\a\b\f Hello World!!!";
    cout << "s before trim:" << s << endl;
    string trim_s = StrUtils::trim(s);
    cout << "s after trim:" << trim_s << endl;

// ----- Testing removeCharFromString ------------------------------------------
    cout << "\n----- Testing removeCharFromString -----\n";
    string remove_s = "Apple Sauce is Super";
    char remove_big_ch = 'S';
    char remove_sml_ch = 's';
    string removed_big;
    removed_big = StrUtils::removeLetterFromString(remove_s, remove_big_ch);
    cout << "String after removing the 'S': " << removed_big << endl;
    string removed_small;
    removed_small = StrUtils::removeLetterFromString(removed_big, remove_sml_ch);
    cout << "Removing little 's': " << removed_small << endl;

// ----- Testing removeLetterFromString ----------------------------------------
	cout << "\n----- Testing removeLetterFromString -----\n";
	string remove_s2 = "Second String Test Apple Sauce is Super 212121";
	char remove_char = 'S';
	string remove_sml_s = "e";
	int remove_int = 1;
	string removed_big_char;
	removed_big_char = StrUtils::removeLetterFromString(remove_s2, remove_char);
	cout << "String after removing the char 'S': " << removed_big_char << endl;
	string removed_str;
	removed_str = StrUtils::removeLetterFromString(remove_s2, remove_sml_s);
	cout << "String after removing the string \"e\": " << removed_str << endl;
	string removed_int;
	removed_int = StrUtils::removeLetterFromString(remove_s2, remove_int);
	cout << "String after removing the int 1: " << removed_int << endl;

// ----- Testing maxValue and minValue -----------------------------------------
    cout << "\n----- Testing maxValue & minValue -----\n";
    double da = 10.1, db = 10.2;
    float fa = 10.0, fb = 10.2;
    char ca = 'a', cb = 'b';
    double d_min = NumUtils::minValue(da, db);
    float f_max = NumUtils::maxValue(fa, fb);
    char c_max = NumUtils::maxValue(ca, cb);
    cout << "d_min: " << d_min << "\tf_max: " << f_max << endl;
    cout << "c_max: " << c_max << endl;

// ----- Testing maxValueVec and minValueVec -----------------------------------
    cout << "\n----- Testing maxValueVec & minValueVec -----\n";
    vector<int> v;
    v.push_back(5);
    v.push_back(6);
    v.push_back(7);
    v.push_back(8);
    v.push_back(9);
    int min_vec = NumUtils::minValueVec(v);
    int max_vec = NumUtils::maxValueVec(v);
    cout << "min_vec: " << min_vec << "\tmax_vec: " << max_vec << endl;
    
// ----- Testing numToString ---------------------------------------------------
    cout << "\n----- Testing numToString -----\n";
    int i_to_s = 5;
    double d_to_s = 5.0;
    float f_to_s = 10.0;
    string s_from_i = StrUtils::numToString(i_to_s);
    string s_from_d = StrUtils::numToString(d_to_s);
    string s_from_f = StrUtils::numToString(f_to_s);
    cout << "int: " << s_from_i << "\tdouble: " << s_from_d <<
    "\tfloat: " << s_from_f << endl;
    
// ----- Testing strToInt ------------------------------------------------------
    cout << "\n----- Testing strToInt -----\n";
    string s_to_i = "2014";
    int i_from_s = NumUtils::strToInt(s_to_i);
    cout << "string_to_int: " << i_from_s << endl;
    
    
// ----- Testing strToDouble ---------------------------------------------------
    cout << "\n----- Testing strToDouble -----\n";
    string s_to_d = "23759.75";
    double d_from_s = NumUtils::strToDouble(s_to_d);
    cout << "string_to_double: " << setprecision(10) << d_from_s << endl;
    
// ----- Testing sumOfDigits ---------------------------------------------------
    cout << "\n----- Testing sumOfDigits -----\n";
    int sum_i = 456;
    cout << "sum of sum_i: " << NumUtils::sumOfDigits(sum_i) << endl;
    
// ----- Testing fibonacci -----------------------------------------------------
    cout << "\n----- Testing fibonacci -----\n";
    int fib_num = 25;
    cout << "fib_num(25) (should output 75025): "
         << NumUtils::fibonacci(fib_num) << endl;
    
// ----- Testing minMaxInVec ---------------------------------------------------
    cout << "\n----- Testing minMaxInVec -----\n";
    vector<int> vec_int;
    for(int i = 0; i < 5; ++i){
        vec_int.push_back(i);
    }
    int begin = 0;
    auto end = (vec_int.size() - 1); //zero index
    vector<double> vec_double;
    for(int i = 0; i < 5; ++i){
        vec_double.push_back(i * 10.17);
    }
    vector<float> vec_float;
    for(int i = 0; i < 5; ++i){
        vec_float.push_back(i * 1.37);
    }
    vector<int> minMaxInt = NumUtils::minMaxInVec(vec_int, begin, end);
    vector<double> minMaxDouble = NumUtils::minMaxInVec(vec_double, begin, end);
    vector<float> minMaxFloat = NumUtils::minMaxInVec(vec_float, begin, end);
    cout << "-- * -- First 3 prints in loop are min value,"
         << " next 3 are max value -- * --\n";
    for(int i = 0; i < minMaxInt.size(); ++i){
        cout << "minMaxInt: " << minMaxInt[i] << endl;
        cout << "minMaxDobule: " << minMaxDouble[i] << endl;
        cout << "minMaxFloat: " << minMaxFloat[i] << endl;
    }
   // ----- Testing toUpper & toLower ------------------------------------------
    cout << "\n----- Testing toUpper & toLower -----\n";
    string sass = "TeSt CaSe StRiNg";
    cout << "First sass, no conversion: " << sass << endl;
    StrUtils::toUpper(sass);
    cout << "Second sass, after toUpper: " << sass << endl;
    StrUtils::toLower(sass);
    cout << "Third sass, after toLower: " << sass << endl;

    
    return 0;
}

