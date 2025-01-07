#include "App.h"
#include <cmath>
using namespace std;

bool splits(std::string line, std::vector<std::string>& container)
{
    std::vector<std::string>& parts = container;

    int start_pos=0;
    int end_pos=-1;

    while(start_pos!=-1)
    {
        end_pos = line.find_first_of(',');
        if (end_pos==-1 && !line.empty())
        {
            parts.push_back(line);
            return true;
        }
        std::string sub_string = line.substr(start_pos,end_pos);
        parts.push_back(sub_string);
        line = line.substr(end_pos+1,line.length()-1);

        if(line.empty())
        {
            return true;
        }
    }

    return false;
}


/**
 * Define all global variables and data arrays
 */
const int ARRAY_LENGTH = 16;
float magnitude[ARRAY_LENGTH];
float diffs[ARRAY_LENGTH];
float squares[ARRAY_LENGTH];
float averages[ARRAY_LENGTH];

/**
 * Enumerate some basic states the machine may wish to follow
 */
enum STATE {IDLE, DETECT, TIMEOUT};
STATE currentState;
STATE nextState;

/**
 * Create a file input stream that we will constantly read from.
 */
ifstream fin;
bool app_init(void)
{
    /**
     * Step 0: Initial all arrays defined above to known values. Really should be 0.
     * Will use FOR LOOP that is very inefficient but is guaranteed to work on all systems
     */

    for(int i=0;i<ARRAY_LENGTH;i++)
    {
        magnitude[i]=0;
        diffs[i]=0;
        squares[i]=0;
        averages[i]=0;
    }

    // Initialize the states of the machine to the initial one
    currentState = IDLE;
    nextState = IDLE;

    /**
     * Step 1: Initialize any data sources that will be needed. When running on PC, this will be the
     * .csv files that contain step data. On the Giant Gecko this will be accessing acceleration data from ADXL345.
     */
    string filePath = "100 Steps Ethan.csv";
    filePath="../"+filePath;
    fin.open(filePath.c_str());

    if(fin.fail())
    {
        cout << "Failed opening file: " << filePath << endl;
        cout << "Could not open file! Check file path. Consider using an absolute path.";
        return false;
    }

    // Proceed to read out the first line of the file in case it holds header information
    string header="";
    fin >> header;
    cout << "Disposing of header: " << header << endl;

    /**
     * Step 2: Initialize any other system level operations: Timers, LCD screen...etc
     */
    // do nothing at this time

    return true;
}

/**
 * Create a counter variable to track how after this loop is called. On the PC, this will correspond to each line
 * of the text file. On the Giant Gecko, this will be each "tick" of the program at each sample.
 */
int counter = 0;
bool app_process_action(void)
{
    /**
     * Step 1: Read data from the source. In this example we will use the file input stream FIN to read from the
     * previously defined CSV file.
     */
    if(!fin.good())
    {
        cout << "We are at the end of file or an error occurred. Exiting." << endl;
        return false;
    }

    // read from file into string
    string line = "";
    fin >> line;

    // check for blank line at end of file...
    if (line=="")
    {
        return false;
    }


    // now split the string based upon comma separators. Will populate a vector that contains
    // all the elements of the line. Vector length adjusts based upon number of columns
    vector<string> line_parts;
    bool success = splits(line,line_parts);

    /**
         * Pull out the X, Y, and Z acceleration and convert to floats.
         * This can be expanded for other data types in the file
         * Each column, for this particular row, now resides as elements of the string
         * Time = [0]
         * AccelX = [1]
         * AccelX = [2]
         * AccelX = [3]
         * GyroX = [4]
         * ... etc
         *
         * If you are operating on a file with a different structure, adjust the values
         * 1, 2, and 3 below to be the columns that hold the X, Y, and Z acceleration.
         * 0 is the "first column", 1 is the "second"..etc.
         */
    float x = atof(line_parts[1].c_str());
    float y = atof(line_parts[2].c_str());
    float z = atof(line_parts[3].c_str());

    /**
     * Step 2: Begin data processing pipeline. The first action is likely to calculate the magnitude
     * of the most recent sample.
     */
    //magnitude
    float mag = sqrt(x*x + y*y + z*z);
    magnitude[counter%ARRAY_LENGTH] = mag;
    //diff
    float d = magnitude[counter%ARRAY_LENGTH]-magnitude[(counter-1)%ARRAY_LENGTH];
    diffs[counter%ARRAY_LENGTH] = d;
    //square
    squares[counter%ARRAY_LENGTH] = d*d;
    //sum squares
    float sum = 0;
    for (int i=0;i<ARRAY_LENGTH;i++) {
        sum += squares[i];
    }
    float averages_sum =sum/ARRAY_LENGTH;
    averages[counter%ARRAY_LENGTH] = averages_sum;

    /**
     * Step 3: Utilize finite state machine to determine whether a step has been detected.
     */
    static int step_counter=0;
    static int time_out=0;
    switch(currentState){

    case(IDLE):
        // do what is needed in the IDLE state. We'll just stay here as an example

        if (averages_sum>0.03) {
            nextState = DETECT;
        }
        else {
            nextState = IDLE;
        }
        break;

        case(DETECT):
        // do what is needed in the DETECT state
        step_counter++;
        time_out=0;
        nextState=TIMEOUT;
        break;

    case(TIMEOUT):
        // do what is needed in the TIMEOUT state
        time_out++;
        if (time_out>7) {
            nextState = IDLE;
        }
        else {
            nextState = TIMEOUT;
        }
        break;

    default:
        cout << "Error! This should never happen!" << endl;
        break;
    }


    // update state machine for next iteration
    currentState = nextState;

    // increment our counter to know how many iterations this has been.
    counter++;
    cout << "Number of steps taken is " << step_counter << endl;
    // return true so the main() method will continue to call us...
    return true;
}
