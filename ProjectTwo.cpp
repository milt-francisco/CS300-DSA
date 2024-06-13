//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Milton Francisco
// Version     : 1.0
// Description : Advising Assistance Program
//============================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <stdexcept>

using namespace std;

// Structure to hold Course information
struct Course {
	string courseNumber;
	string courseTitle;
	vector<string> prereqs;
};

// Forward declaration
void displayCourse(const Course&);

/*
* This function verifies the file is formatted correctly as follows:
*
* - Checks for at least 2 parameters in a line
* - Checks that a prerequisite exists as a course in the file
* - If a prereq does not have a corresponding course in the file, outputs applicable prereqs that need course info
*/
void verifyFileFormat(const string& file) {
	// Create an input file pointer
	ifstream input;
	// Open the file
	input.open(file);

	// Check that file was opened correctly
	if (!input.is_open()) {
		// If not opened correctly, throw error
		throw runtime_error("Could not open input file.");
	}

	//checkAndRemoveBOM(input);

	// Vectors to hold information from file
	vector<string> courseNumbers;
	vector<string> prereqs;

	// String to hold line from csv file
	string line;

	// Obtain lines from csv file
	while (getline(input, line)) {
		// stringstream to process each line
		stringstream ss(line);

		// Variable to hold each token in the line
		string token;

		// Temp vector to hold tokens from the current line
		vector<string> temp;

		// Loop while there is a word in the line 
		while (getline(ss, token, ',')) {
			// Push the word to the temp vector
			temp.push_back(token);
		}

		// Check if the temp vector has at least 2 parameters
		if (temp.size() < 2) {
			throw runtime_error("Improper file format: There are not at least 2 parameters on each line.");
		}

		// If the temp vector has at least 2 parameters, push to applicable vectors
		else {
			// First string (from the line) is the courseNumber
			courseNumbers.push_back(temp.at(0));

			// Skip second string, as this is the title and is currently irrelevant, start at 3rd string (index 2)
			for (unsigned int i = 2; i < temp.size(); ++i) {

				// The remaining values are prereqs to be pushed to applicable vector
				if (!temp.at(i).empty()) { //Ensure value is not empty
					prereqs.push_back(temp.at(i));
				}
			}
		}
	}
	// File has been parsed and applicable information gleaned
	input.close();

	// Vector to hold prereqs that do not have courses in the file
	vector<string> missingPrereqs;

	// Verify prereq exists as a course from the file (separate loop in case courses out of order)
	for (string prereq : prereqs) {
		// Checks if prereq is in courseNumbers vector by counting occurences
		int prereqCheck = count(courseNumbers.begin(), courseNumbers.end(), prereq);

		// If prereq is not in the vector
		if (prereqCheck == 0) {
			// Output which prereqs do not have a corresponding course in file
			missingPrereqs.push_back(prereq);
		}
	}

	// If there are missing prereqs from file
	if (!missingPrereqs.empty()) {

		// Output which prereqs do not have corresponding courses in the file.
		cout << "Missing prereq: " << endl;
		for (string prereq : missingPrereqs) {
			cout << prereq << ", ";
		}
		cout << "as a course." << endl;

		// Throw error to be caught by catch block.
		throw runtime_error("Improper file format: Prereq(s) exist without a corresponding course in the file.");
	}

	// If no errors are found, no exceptions are thrown and the function returns normally.
	return;
}

/*
* Function to load courses into data structure
* @param string, vector passed by reference
*
* -Used after the file is verified with verifyFileFormat()
*/
vector<Course>& loadCourses(const string& fileName, vector<Course>& courses) {
	// Open file
	ifstream input;
	input.open(fileName);

	// Variable to hold a single line from the file
	string line;

	// Obtain lines from file and extract information
	while (getline(input, line)) {
		// Create Course object to hold information
		Course course;

		// Use stringstream to process each line
		stringstream ss(line);

		// Variable to hold each token in the line
		string token;

		// Obtain first string as courseNumber
		getline(ss, course.courseNumber, ',');

		// Obtain second string as title
		getline(ss, course.courseTitle, ',');

		// Loop while prereqs remain in the line and push to prereqs vector
		while (getline(ss, token, ',')) {
			if (token != "") { // Ensure prereq is not an empty string
				course.prereqs.push_back(token);
			}
		}

		//Push course to courses vector
		courses.push_back(course);
	}

	// Close file
	input.close();

	return courses;
}

/*
* This function prints all of the courses in order
*/
void printCourses(const vector<Course>& courses) {
	// Loop through courses, (vector is already sorted)
	for (Course course : courses) {
		// Output information to user (only courseNumber and title)
		cout << course.courseNumber << ", " << course.courseTitle << endl;
	}
}

/*
* Function searches for a specific course based off of the courseNumber
*/
void searchCourse(const vector<Course>& courses, const string& courseNumber) {
	// Loop through all courses
	for (Course course : courses) {
		// Check if courseNumbers match
		if (course.courseNumber.compare(courseNumber) == 0) {
			// Match is found and course is displayed
			displayCourse(course);

			// Exit function early to prevent not found message displaying
			return;
		}
	}

	// If course was not found, output not found message
	cout << "Course " << courseNumber << " not found." << endl;

	return;
}

/*
* Function displays all course information from a passed in Course object
*/
void displayCourse(const Course& course) {
	//Output information to user
	cout << course.courseNumber << ", " << course.courseTitle << endl;
	cout << "Prerequisites: ";

	// Check if there are prerequisites
	if (course.prereqs.empty()) {
		cout << "None." << endl;

		// Exit early since there are no prereqs
		return;
	}

	// Loop through prereqs vector
	for (unsigned int i = 0; i < course.prereqs.size(); ++i) {
		//Output information to user
		cout << course.prereqs.at(i);

		// if more than one prereq, output comma after each except last prereq
		if (i < course.prereqs.size() - 1) {
			cout << ", ";
		}
	}
	cout << endl;
	return;
}

/*
* Helper function to sort the courses in vector by courseNumber
* Tells sort() function to place course2 before or after course1
*/
bool compareCourses(const Course& course1, const Course& course2) {
	// If course 2 letters or numbers are larger than course1, return true
	if (course1.courseNumber.compare(course2.courseNumber) < 0) {
		return true;
	}
	return false;
}


int main()
{
	// Variable to hold applicable file name
	string csvPath;

	// Welcome message
	cout << string(31, '*') << endl;
	cout << "* Advising Assistance Program *" << endl;
	cout << string(31, '*') << endl << endl;

	//Loop for user input (in case user mis-inputs file/file is incorrect/or testing an improper format file)
	while (1) {

		// Prompt and obtain file name
		cout << "Please enter the file name that contains the course data (\"\" not required): ";
		getline(cin, csvPath); // Changed to this (from cin >> csvPath) in order to obtain filename with spaces

		// Try to verify file format
		try {
			//If user wants to exit, exit program.
			if (csvPath == "exit" || csvPath == "EXIT" || csvPath == "Exit") {
				cout << "Good bye." << endl;
				return 1;
			}

			verifyFileFormat(csvPath);
			break;
		}
		catch (exception& exc) {
			// If it is an incompatible file, this shows why it is incompatible
			cout << exc.what() << endl << endl;
			cout << "If you wish to exit, enter exit" << endl << endl;
		}
	}

	// Variable for menu choice
	int userChoice = 0;

	// Variable for specific course chosen
	string courseChoice = "";

	//Implement data structure
	vector<Course> courses;

	cout << endl << "Welcome to the course planner." << endl;
	while (userChoice != 9) {

		cout << endl << "  1. Load Data Structure." << endl;
		cout << "  2. Print Course List." << endl;
		cout << "  3. Print Course." << endl;
		cout << "  9. Exit" << endl << endl;

		cout << "What would you like to do? ";
		cin >> userChoice;

		switch (userChoice) {
		case 1:
			// Load courses into data structure
			loadCourses(csvPath, courses);

			// Sort vector now for use later
			sort(courses.begin(), courses.end(), compareCourses);

			break;

		case 2:
			//Display all courses in order
			cout << "Here is a sample schedule: " << endl << endl;
			printCourses(courses);

			break;

		case 3:
			//Prompt user and obtain specific course
			cout << "What course do you want to know about? ";
			cin >> courseChoice;
			cout << endl;

			// Make courseChoice case insensitive. 
			// Reference: https://www.geeksforgeeks.org/conversion-whole-string-uppercase-lowercase-using-stl-c/
			transform(courseChoice.begin(), courseChoice.end(), courseChoice.begin(), ::toupper);

			// Search for the courseChoice in courses
			searchCourse(courses, courseChoice);

			break;

		case 9:
			//Prevents user from seeing "invalid option" message when exiting program.
			break;

		default:
			// Prevents improper usage and informs user their selection was not an option 
			cout << userChoice << " is not a valid option." << endl;	// Displays random number if input is not an int
			cin.clear();												// Clears instream
			cin.ignore(UINT_MAX, '\n');									// Ignores user input to prevent infinite loop
			break;
		}
	}
	cout << "Thank you for using the course planner!" << endl;

	return 0;
}

