/*******************************************************************************
*                                                                              *
* Author    :  Kenny Trussell                                                  *
* Version   :  1.0.0                                                           *
* Date      :  1 July 2018                                                    *
* Youtube Ch:  https://www.youtube.com/channel/UCkIJQFyZdzLKh7AR-m3t2Bw        *
* Copyright :  Kenny Trussell 2018                                             *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to MIT License                   *
* https://opensource.org/licenses/MIT                                          *
*                                                                              *
*******************************************************************************/

// Description: This program shifts all waypoints by a single latitude/longitude offset.
// The offset is defined by the difference in the last waypoint and the next-to-last waypoint in the file.
// The last waypoint is NOT replicated to the new output file. It is purely placed in the input file to "show"
// the program how much to offset the other waypoints.

//Command line example: WPShift.exe inwaypointfilename outwaypointfilename

#define NDEBUG	//Comment out this line to enable additional print statements for debugging purposes

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

const int         MAXLINES     = 1000;			//Maximum number of lines in an input waypoint file
const int         PARMSPERLINE = 12;			//Number of parameters in each line of waypoint file (except 1st line)

int main (int argc, char ** argv)
{
	std::string FirstLine, SecondLine, line;

	//Command line arguments:
	// argv[1]: input waypoint filename
	// argv[2]: output waypoint filename

	if (argc != 3)
	{
		std::cerr << "Usage:\n WPShift.exe InputWayPointFileName OutputWayPointFileName" << std::endl;
		return 1;
	}

	std::string inputname   = argv[1];
	std::string outname     = argv[2];

	std::ifstream infile(inputname);
	if (!infile)
	{
		std::cerr << "Unable to open input file: " << inputname << std::endl;
		return 2;
	}

	std::ofstream outfile(outname);
	if (!outfile)
	{
		std::cerr << "Unable to open output file: " << outname << std::endl;
		return 3;
	}

	std::string WPparms[MAXLINES][PARMSPERLINE];

	//Read 1st and 2nd lines and keep for writing unchanged to output file.
	std::getline(infile, FirstLine);
	std::getline(infile, SecondLine);

	// Create the output file and write the 1st two lines from the input file.
	outfile << FirstLine << std::endl << SecondLine << std::endl;

	unsigned lno = 0;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		for (int icnt = 0; icnt < PARMSPERLINE; icnt++) //Parse the line to get indiviual values.
			iss >> WPparms[lno][icnt];
		lno++;
	}

	#ifndef NDEBUG
		for (int jcnt = 0; jcnt < lno; jcnt++)
		{
			for (int icnt = 0; icnt < PARMSPERLINE; icnt++)
			{
				std::cout << WPparms[jcnt][icnt] << " ";
			}
			std::cout << std::endl;
		}
	#endif

	// Subtract the lat and long of the last entry from the  next-to-last entry. This is the offset to add to each entry to make
	// the new file.
	long double LatShift = std::stod(WPparms[lno-2][8]) - std::stod(WPparms[lno-1][8]);
	long double LongShift = std::stod(WPparms[lno-2][9]) - std::stod(WPparms[lno-1][9]);

	for(int icnt = 0; icnt < lno-1; icnt++)
	{
		for (int kcnt = 0; kcnt < 8; kcnt++)
		{
			outfile << WPparms[icnt][kcnt] << " ";
		}

    outfile.precision(9); //We want lat and long to have 8 digits + decimal place.
			//(long double) used to hold the "scaled back to decimal" values of the (int64_t) integer values.
    outfile << std::stod(WPparms[icnt][8]) - LatShift << " ";
    outfile << std::stod(WPparms[icnt][9]) - LongShift << " ";

    outfile << WPparms[lno-1][10] << " " << WPparms[lno-1][11] << std::endl;
  }

	outfile.close();
	std::cout << "Finished." << std::endl;

    return 0;
}
