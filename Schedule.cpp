// include "ScheduleWhen.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <utility>
using namespace std;

class Schedule
{
private:
	int totalRooms;



	//setting up a timetable with full of -1
	vector< vector<int> > generateGenericTimetable(int noLecturers)
	{
		vector< vector<int> > genericTimetable;

		for (int i=0; i<noLecturers; i++) {
			genericTimetable.push_back({});
			for (int m=0; m<40; m++) {
				genericTimetable[i].push_back(-1);
			}
		}

		return genericTimetable;
	}



	//create a vector of list of courses that a lecturer will teach
	vector< vector<int> > generateTeachingCourses(vector< vector<int> > &binaryMapping)
	{
		vector< vector<int> > teachingCourses;

		for (int i=0; i<binaryMapping[0].size(); i++) {			//all the lecturers
			teachingCourses.push_back({});

			for (int m=0; m<binaryMapping.size(); m++) {		//if the lecturer teaches that course, add to the list
				if (binaryMapping[m][i] == 1) {
					teachingCourses[i].push_back(m);
				}
			}
		}


		return teachingCourses;
	}




	//determining if the lecturer can teach at this session, and if can, decides to teach or not
	void generateLecturer(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, vector< vector<int> > timeTable)
	{
		if (lecturerNo >= preference.size()) {				//every lecturer has been generated
			for (int i=0; i<timeTable.size(); i++) {			//just print everything out for testing purposes
				for (int m=0; m<timeTable[i].size(); m++) {
					if (timeTable[i][m] != -1) {
						cout << "\e[34m";
					}
					cout << timeTable[i][m] << "\e[0m ";
				}
				cout << endl;
			}
			cout << endl;
			for (int i=0; i<hours.size(); i++) {			//invalid option: doesn't teach all the required hours
				if (hours[i] > 0) {
					return ;
				}
			}

			checkRoom(timeTable);			//check if the number of rooms allows for this timetable

		} else if (session >= 40) {			//all 40 hours have been checked
			generateLecturer(teachingCourses, hours, preference, 0, lecturerNo + 1, timeTable);

		} else if (preference[lecturerNo][session] == 0) {			//if lecturer is not available for teaching at that session
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable);
			
		} else {			//lecturer can teach in that session
			//if the lecturer decides to teach in that session
			generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, 0, timeTable);

			//if lecturer doesn't teach in that session, then move on to the next session
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable);

		}
	}






	//if the lecturer decides to teach, which course to teach (since a lecturer can tech multiple courses)
	void generateCourseToTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable)
	{
		// if there is no hour left to teach in this course then teach the next one
		if (hours[courseNo] == 0) {		
			generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo + 1, timeTable);

		} else if (hours[teachingCourses[lecturerNo].size()] == 0) {			//if the last course doesn't have any time left to teach that means the lecturer is done for the week
			generateLecturer(teachingCourses, hours, preference, 0, lecturerNo + 1, timeTable);

		} else {		//if there are still hours left in that course

			//if this is not the final course, the lecturer has the choice to either teach it or not
			if (courseNo < teachingCourses[lecturerNo].size() - 1) {				//if the lecturer chooses not to teach it
				generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo + 1, timeTable);
			}
			generateTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo, timeTable);			//if lecturer chooses to teach it

		}		
	}





	//this fucntion will determine if the lecturer can and will teach 2 hour session or not
	void generateTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable)
	{
		timeTable[lecturerNo][session] = teachingCourses[lecturerNo][courseNo];			//since you'll have to teach at that session, modify the time table at session to mark that you'll teach at that time
		hours[courseNo]--;						//and subtract 1 to the remaining hour of that course
		generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable);


		// these if conditions will see if the lecturer can teach two hour sessions or not
		if (preference[lecturerNo][session + 1] != 0 && hours[courseNo] > 1) {		// if lecturer is free next hour (or it's lunch break), he/she can teach 2 hours in a row
			if (session % 8 == 7) {			// if the next hour is end of day (or, this session is the last hour of the day), then do nothing
				return;	

			} else if (session % 8 == 6 || session % 8 == 3) {
				// if the next hour is the last hour of the day, then just move on to the next session (i.e. start of new day)
				// if the next hour is lunch break, then just jump an hour ahead since lunch break doesn't count in schedule
				hours[courseNo]--;
				timeTable[lecturerNo][session + 1] = teachingCourses[lecturerNo][courseNo];
				generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable);

			} else {							// if the next hour is not the last hour of the day, then you have to jump 1 more hour as teaching break
				hours[courseNo]--;
				timeTable[lecturerNo][session + 1] = teachingCourses[lecturerNo][courseNo];
				generateLecturer(teachingCourses, hours, preference, session + 3, lecturerNo, timeTable);			
			}
		}
		
	}



	void checkRoom(vector< vector<int> > &timeTable) 
	{
		int counter = 0;
		for (int i=0; i<timeTable[0].size(); i++) {				//check the timeTable and see if there are more than 'rooms' number of courses being taught at a time
			for (int m=0; m<timeTable.size(); m++) {
				if (timeTable[m][i] != -1) {
					counter++;
				}
			}

			if (counter > totalRooms) {
				return;
			}
			counter = 0;
		}

		//next function here
	}





public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		totalRooms = rooms;
		vector< vector<int> > teachingCourses = generateTeachingCourses(binaryMapping);
		for (int i=0; i<teachingCourses.size(); i++) {
			for (int m=0; m<teachingCourses[i].size(); m++) {
				cout << teachingCourses[i][m] << " ";
			}
			cout << endl;
		}
		cout << endl;
		vector< vector<int> > genericTimetable = generateGenericTimetable(lecturers.size());

		generateLecturer(teachingCourses, hours, preferences, 0, 0, genericTimetable);			//the start of it all

	}
	
};