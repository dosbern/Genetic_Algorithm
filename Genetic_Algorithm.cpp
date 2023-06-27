// Genetic_Algorithm.cpp
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

const int numberOfPopulation = 500;
const int numberOfCyclesForOneGeneration = 100;
const int numberOfGenerations = 100;

const vector <pair<string, int>> rooms = {
        {"Slater 003", 45},
        {"Roman 201", 50},
        {"Roman 216", 30},
        {"Loft 206", 75},
        {"Loft 310", 108},
        {"Beach 201", 60},
        {"Beach 301", 75},
        {"Logos 325", 450},
        {"Frank 119", 60}
};
const vector <string> times = {
        "10 AM",
        "11 AM",
        "12 PM",
        "1 PM",
        "2 PM",
        "3 PM"
};
const  vector<string> facilitators = {
        "Lock",
        "Glen",
        "Banks",
        "Richards",
        "Shaw",
        "Singer",
        "Uther",
        "Tyler",
        "Numen",
        "Zeldin"
};

// Data
class activity {
public:
    string name;
    string room;
    int roomCapacity = 0;
    string time;
    string facilitator;
    int expectedEnrollment = 0;
    list <string> preferredFacilitator;
    list <string> normalFacilitator;
    list <string> otherFacilitator;
    double fitnessScore = 0;
};

class schedule {
public:
    schedule() {
        //empty constuctor
    }
    double fitnessScore = 0;
    vector<activity> act;
    int value;
    schedule(int val) : value(val) {}
};

struct CompareMyObjects {
    bool operator()(schedule const& obj1, schedule const& obj2) {
        return obj1.fitnessScore > obj2.fitnessScore;
    }
};

// Funtions
void generateActivity(activity& act) {
    int randIndex = rand() % rooms.size();
    act.room = rooms.at(randIndex).first;
    act.roomCapacity = rooms.at(randIndex).second;
    randIndex = rand() % times.size();
    act.time = times.at(randIndex);
    randIndex = rand() % facilitators.size();
    act.facilitator = facilitators.at(randIndex);
}

bool operator==(const std::string& str1, const std::string& str2) {
    return str1.compare(str2) == 0;
}

void calculateFitnessOneActivity(activity& act) {
    act.fitnessScore = 0;
    // room capacity
    if (act.roomCapacity < act.expectedEnrollment) {
        act.fitnessScore -= 0.5;
    }
    else if (act.roomCapacity > (act.expectedEnrollment * 3)) {
        act.fitnessScore -= 0.2;
    }
    else if (act.roomCapacity > (act.expectedEnrollment * 6)) {
        act.fitnessScore -= 0.4;
    }
    string item = act.facilitator;
    // preferred facilitator
    for (string item : act.preferredFacilitator) {
        if (item == act.facilitator) {
            act.fitnessScore += 0.5;
        }
    }
    // normal facilitator
    for (string item : act.normalFacilitator) {
        if (item == act.facilitator) {
            act.fitnessScore += 0.2;
        }
    }
    // other facilitator
    for (string item : act.otherFacilitator) {
        if (item == act.facilitator) {
            act.fitnessScore -= 0.1;
        }
    }
}

int removeLettersAndConvertToInt(const string& str) {
    // Remove all non-digit characters from the string
    string digitsOnly = "";
    remove_copy_if(str.begin(), str.end(), back_inserter(digitsOnly), [](char c) {
        return !isdigit(c);
        });

    // Convert the resulting string to an integer
    return stoi(digitsOnly);
}

void calculateFitnessAllActivities(schedule& sched) {
    sched.fitnessScore = 0;
    // same time and same room as another activity
    for (size_t i = 0; i < sched.act.size(); i++) {
        if (i < sched.act.size() - 1) {
            if (sched.act[i].time == sched.act[i + 1].time) {
                if (sched.act[i].room == sched.act[i + 1].room) {
                    sched.fitnessScore -= 0.5;
                }
            }
        }
    }
    // number of facilitator scheduled activities
    int numberOfActivitiesAtSameTime = 0;
    int activitiesAt10AM = 0;
    int activitiesAt11AM = 0;
    int activitiesAt12PM = 0;
    int activitiesAt1PM = 0;
    int activitiesAt2PM = 0;
    int activitiesAt3PM = 0;
    vector<string> locationAt10AM;
    vector<string> locationAt11AM;
    vector<string> locationAt12PM;
    vector<string> locationAt1PM;
    vector<string> locationAt2PM;
    vector<string> locationAt3PM;
    for (size_t i = 0; i < facilitators.size(); i++) {
        if (i < facilitators.size() - 1) {
            for (size_t j = 0; j < sched.act.size(); j++) {
                if (j < sched.act.size() - 1) {
                    if (facilitators[i] == sched.act[j].facilitator) {
                        if (sched.act[j].time == "10 AM") {
                            activitiesAt10AM++;
                            locationAt10AM.push_back(sched.act[j].room);
                        }
                        if (sched.act[j].time == "11 AM") {
                            activitiesAt11AM++;
                            locationAt11AM.push_back(sched.act[j].room);
                        }
                        if (sched.act[j].time == "12 PM") {
                            activitiesAt12PM++;
                            locationAt12PM.push_back(sched.act[j].room);
                        }
                        if (sched.act[j].time == "1 PM") {
                            activitiesAt1PM++;
                            locationAt1PM.push_back(sched.act[j].room);
                        }
                        if (sched.act[j].time == "2 PM") {
                            activitiesAt2PM++;
                            locationAt2PM.push_back(sched.act[j].room);
                        }
                        if (sched.act[j].time == "3 PM") {
                            activitiesAt3PM++;
                            locationAt3PM.push_back(sched.act[j].room);
                        }
                    }
                }
            }
        }
        // add up number of activites scheduled at the same time
        if (activitiesAt10AM > 1) {
            numberOfActivitiesAtSameTime += activitiesAt10AM;
        }
        if (activitiesAt10AM == 1) {
            sched.fitnessScore += 0.2;
        }
        if (activitiesAt11AM > 1) {
            numberOfActivitiesAtSameTime += activitiesAt11AM;
        }
        if (activitiesAt11AM == 1) {
            sched.fitnessScore += 0.2;
        }
        if (activitiesAt12PM > 1) {
            numberOfActivitiesAtSameTime += activitiesAt12PM;
        }
        if (activitiesAt12PM == 1) {
            sched.fitnessScore += 0.2;
        }
        if (activitiesAt1PM > 1) {
            numberOfActivitiesAtSameTime += activitiesAt1PM;
        }
        if (activitiesAt1PM == 1) {
            sched.fitnessScore += 0.2;
        }
        if (activitiesAt2PM > 1) {
            numberOfActivitiesAtSameTime += activitiesAt2PM;
        }
        if (activitiesAt2PM == 1) {
            sched.fitnessScore += 0.2;
        }
        if (activitiesAt3PM > 1) {
            numberOfActivitiesAtSameTime += activitiesAt3PM;
        }
        if (activitiesAt3PM == 1) {
            sched.fitnessScore += 0.2;
        }
        //facilitator scheduled for more than one activity at the same time
        sched.fitnessScore = sched.fitnessScore + (numberOfActivitiesAtSameTime * -0.2);
        //facilitator scheduled for more than four activities
        if (numberOfActivitiesAtSameTime > 4) {
            sched.fitnessScore -= 0.5;
        }
        //facilitator scheduled for one or two activites unless its Tyler
        if ((numberOfActivitiesAtSameTime == 1 or numberOfActivitiesAtSameTime == 2) && facilitators[i] != "Tyler") {
            sched.fitnessScore -= 0.4;
        }
        // facilitator scheduled for consecutive time slots
        bool consecutive = false;
        if (activitiesAt10AM > 0 && activitiesAt11AM > 0) {
            for (auto it1 = locationAt10AM.begin(); it1 != locationAt10AM.end(); it1++) {
                for (auto it2 = locationAt11AM.begin(); it2 != locationAt11AM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt11AM > 0 && activitiesAt12PM > 0) {
            for (auto it1 = locationAt11AM.begin(); it1 != locationAt11AM.end(); it1++) {
                for (auto it2 = locationAt12PM.begin(); it2 != locationAt12PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt12PM > 0 && activitiesAt1PM > 0) {
            for (auto it1 = locationAt12PM.begin(); it1 != locationAt12PM.end(); it1++) {
                for (auto it2 = locationAt1PM.begin(); it2 != locationAt1PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt1PM > 0 && activitiesAt2PM > 0) {
            for (auto it1 = locationAt1PM.begin(); it1 != locationAt1PM.end(); it1++) {
                for (auto it2 = locationAt2PM.begin(); it2 != locationAt2PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt2PM > 0 && activitiesAt3PM > 0) {
            for (auto it1 = locationAt2PM.begin(); it1 != locationAt2PM.end(); it1++) {
                for (auto it2 = locationAt3PM.begin(); it2 != locationAt3PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        // if facilitator is scheduled for roman and beach at the same time
        if (activitiesAt10AM > 1) {
            for (auto it1 = locationAt10AM.begin(); it1 != locationAt10AM.end(); it1++) {
                for (auto it2 = locationAt10AM.begin(); it2 != locationAt10AM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt11AM > 1) {
            for (auto it1 = locationAt11AM.begin(); it1 != locationAt11AM.end(); it1++) {
                for (auto it2 = locationAt11AM.begin(); it2 != locationAt11AM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt12PM > 1) {
            for (auto it1 = locationAt12PM.begin(); it1 != locationAt12PM.end(); it1++) {
                for (auto it2 = locationAt12PM.begin(); it2 != locationAt12PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt1PM > 1) {
            for (auto it1 = locationAt1PM.begin(); it1 != locationAt1PM.end(); it1++) {
                for (auto it2 = locationAt1PM.begin(); it2 != locationAt1PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt2PM > 1) {
            for (auto it1 = locationAt2PM.begin(); it1 != locationAt2PM.end(); it1++) {
                for (auto it2 = locationAt2PM.begin(); it2 != locationAt2PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        if (activitiesAt3PM > 1) {
            for (auto it1 = locationAt3PM.begin(); it1 != locationAt3PM.end(); it1++) {
                for (auto it2 = locationAt3PM.begin(); it2 != locationAt3PM.end(); it2++) {
                    if ((*it1 == "Roman 201" or *it1 == "Roman 216") && (*it2 == "Beach 201" or *it2 == "Beach301")) {
                        sched.fitnessScore -= 0.4;
                        consecutive = true;
                    }
                }
            }
            if (consecutive == false) {
                sched.fitnessScore += 0.5;
            }
            consecutive = false;
        }
        // reset all values
        numberOfActivitiesAtSameTime = 0;
        activitiesAt10AM = 0;
        activitiesAt11AM = 0;
        activitiesAt12PM = 0;
        activitiesAt1PM = 0;
        activitiesAt2PM = 0;
        activitiesAt3PM = 0;
        locationAt10AM.clear();
        locationAt11AM.clear();
        locationAt12PM.clear();
        locationAt1PM.clear();
        locationAt2PM.clear();
        locationAt3PM.clear();
    }
    //SLA100A/B and SLA191A/B
    int timeSLA100A = removeLettersAndConvertToInt(sched.act[0].time);
    int timeSLA100B = removeLettersAndConvertToInt(sched.act[1].time);
    int timeSLA191A = removeLettersAndConvertToInt(sched.act[2].time);
    int timeSLA191B = removeLettersAndConvertToInt(sched.act[3].time);
    string locationSLA100A = sched.act[0].room;
    string locationSLA100B = sched.act[1].room;
    string locationSLA191A = sched.act[2].room;
    string locationSLA191B = sched.act[3].room;
    if (timeSLA100A < 10) {
        timeSLA100A += 12;
    }
    if (timeSLA100B < 10) {
        timeSLA100B += 12;
    }
    if (timeSLA191A < 10) {
        timeSLA191A += 12;
    }
    if (timeSLA191B < 10) {
        timeSLA191B += 12;
    }
    //if SLA100A and SLA100B are more than 4 hours apart
    if (abs(timeSLA100A - timeSLA100B) > 4) {
        sched.fitnessScore += 0.5;
    }
    //if SLA191A and SLA191B are more than 4 hours apart
    if (abs(timeSLA191A - timeSLA191B) > 4) {
        sched.fitnessScore += 0.5;
    }
    //if SLA100A and SLA100B are at the same time
    if (timeSLA100A == timeSLA100B) {
        sched.fitnessScore -= 0.5;
    }
    //if SLA191A and SLA191B are at the same time
    if (timeSLA191A == timeSLA191B) {
        sched.fitnessScore -= 0.5;
    }
    //if SLA100A and SLA191A are one hour apart
    if (abs(timeSLA100A - timeSLA191A) == 1) {
        sched.fitnessScore += 0.5;
    }
    //if SLA100A and SLA191B are one hour apart
    if (abs(timeSLA100A - timeSLA191B) == 1) {
        sched.fitnessScore += 0.5;
    }
    //if SLA100B and SLA191A are one hour apart
    if (abs(timeSLA100B - timeSLA191A) == 1) {
        sched.fitnessScore += 0.5;
    }
    //if SLA100B and SLA191B are one hour apart
    if (abs(timeSLA100B - timeSLA191B) == 1) {
        sched.fitnessScore += 0.5;
    }
    //if SLA100A and SLA191A are at the same time
    if (timeSLA100A == timeSLA191A) {
        sched.fitnessScore -= 0.5;
    }
    //if SLA100A and SLA191B are at the same time
    if (timeSLA100A == timeSLA191B) {
        sched.fitnessScore -= 0.5;
    }
    //if SLA100B and SLA191A are at the same time
    if (timeSLA100B == timeSLA191A) {
        sched.fitnessScore -= 0.5;
    }
    //if SLA100B and SLA191B are at the same time
    if (timeSLA100B == timeSLA191B) {
        sched.fitnessScore -= 0.5;
    }
    //if SLA100A/B and SLA191A/B are in consecutive time slots
    if ((timeSLA100A + 1) == timeSLA191A) {
        if (locationSLA100A == "Roman" && locationSLA191A == "Beach" or
            locationSLA100A == "Beach" && locationSLA191A == "Roman") {
            sched.fitnessScore -= 0.4;
        }
    }
    if ((timeSLA100A + 1) == timeSLA191B) {
        if (locationSLA100A == "Roman" && locationSLA191B == "Beach" or
            locationSLA100A == "Beach" && locationSLA191B == "Roman") {
            sched.fitnessScore -= 0.4;
        }
    }
    if ((timeSLA100B + 1) == timeSLA191A) {
        if (locationSLA100B == "Roman" && locationSLA191A == "Beach" or
            locationSLA100B == "Beach" && locationSLA191A == "Roman") {
            sched.fitnessScore -= 0.4;
        }
    }
    if ((timeSLA100B + 1) == timeSLA191B) {
        if (locationSLA100B == "Roman" && locationSLA191B == "Beach" or
            locationSLA100B == "Beach" && locationSLA191B == "Roman") {
            sched.fitnessScore -= 0.4;
        }
    }

    sched.fitnessScore = (sched.fitnessScore +
        sched.act[0].fitnessScore +
        sched.act[1].fitnessScore +
        sched.act[2].fitnessScore +
        sched.act[3].fitnessScore +
        sched.act[4].fitnessScore +
        sched.act[5].fitnessScore +
        sched.act[6].fitnessScore +
        sched.act[7].fitnessScore +
        sched.act[8].fitnessScore +
        sched.act[9].fitnessScore +
        sched.act[10].fitnessScore
        );
}

void populateHeap(activity& SLA100A, activity& SLA100B, activity& SLA191A, activity& SLA191B,
    activity& SLA201, activity& SLA291, activity& SLA303, activity& SLA304, activity& SLA394,
    activity& SLA449, activity& SLA451, priority_queue<schedule, vector<schedule>, CompareMyObjects>& minHeap, int numberOfPopulation) {
    for (int i = 0; i <= numberOfPopulation; i++) {
        schedule sched(i);

        generateActivity(SLA100A);
        calculateFitnessOneActivity(SLA100A);
        sched.act.push_back(SLA100A);

        generateActivity(SLA100B);
        calculateFitnessOneActivity(SLA100B);
        sched.act.push_back(SLA100B);

        generateActivity(SLA191A);
        calculateFitnessOneActivity(SLA191A);
        sched.act.push_back(SLA191A);

        generateActivity(SLA191B);
        calculateFitnessOneActivity(SLA191B);
        sched.act.push_back(SLA191B);

        generateActivity(SLA201);
        calculateFitnessOneActivity(SLA201);
        sched.act.push_back(SLA201);

        generateActivity(SLA291);
        calculateFitnessOneActivity(SLA291);
        sched.act.push_back(SLA291);

        generateActivity(SLA303);
        calculateFitnessOneActivity(SLA303);
        sched.act.push_back(SLA303);

        generateActivity(SLA304);
        calculateFitnessOneActivity(SLA304);
        sched.act.push_back(SLA304);

        generateActivity(SLA394);
        calculateFitnessOneActivity(SLA394);
        sched.act.push_back(SLA394);

        generateActivity(SLA449);
        calculateFitnessOneActivity(SLA449);
        sched.act.push_back(SLA449);

        generateActivity(SLA451);
        calculateFitnessOneActivity(SLA451);
        sched.act.push_back(SLA451);

        sched.fitnessScore = (sched.fitnessScore +
            SLA100A.fitnessScore +
            SLA100B.fitnessScore +
            SLA191A.fitnessScore +
            SLA191B.fitnessScore +
            SLA201.fitnessScore +
            SLA291.fitnessScore +
            SLA303.fitnessScore +
            SLA304.fitnessScore +
            SLA394.fitnessScore +
            SLA449.fitnessScore +
            SLA451.fitnessScore
            );

        calculateFitnessAllActivities(sched);

        minHeap.push(sched);
    }
}

void makeChildren(priority_queue<schedule, vector<schedule>, CompareMyObjects>& minHeap, priority_queue<schedule, vector<schedule>, CompareMyObjects>& minHeapTwo) {
    vector<schedule> tempHeap;
    
    minHeap.pop();
    minHeap.pop();

    schedule parentOne;
    schedule parentTwo;

    bool parentOneFound = false;
    bool parentTwoFound = false;

    int randomIntA = rand() % minHeap.size();
    int randomIntB = rand() % minHeap.size();

    if (randomIntA == randomIntB) {
        randomIntB = rand() % minHeap.size();
    }
    if (randomIntA == randomIntB) {
        randomIntB = rand() % minHeap.size();
    }

    int parentOneInt = 0;
    int parentTwoInt = 1;

    if (randomIntA < randomIntB) {
        parentOneInt = randomIntA;
        parentTwoInt = randomIntB;
    }
    else {
        parentOneInt = randomIntB;
        parentTwoInt = randomIntA;
    }
    
    int currentIndex = 0;

    while (!parentOneFound) {
        schedule topSchedule = minHeap.top();
        tempHeap.push_back(topSchedule);
        minHeap.pop();

        if (currentIndex == parentOneInt) {
            parentOne = topSchedule;
            parentOneFound = true;
        }

        currentIndex++;
    }

    while (!parentTwoFound) {
        schedule topSchedule = minHeap.top();
        tempHeap.push_back(topSchedule);
        minHeap.pop();

        if (currentIndex == parentTwoInt) {
            parentTwo = topSchedule;
            parentTwoFound = true;
        }

        currentIndex++;
    }

    for (auto it = tempHeap.begin(); it != tempHeap.end(); ++it) {
        schedule newSchedule = *it;
        minHeap.push(newSchedule);
    }
    //tempHeap.clear();

    currentIndex = 0;

    schedule childOne = parentOne;
    schedule childTwo = parentTwo;

    // crossover
    childOne.act[6] = parentTwo.act[6];
    childOne.act[7] = parentTwo.act[7];
    childOne.act[8] = parentTwo.act[8];
    childOne.act[9] = parentTwo.act[9];
    childOne.act[10] = parentTwo.act[10];

    childTwo.act[0] = parentOne.act[0];
    childTwo.act[1] = parentOne.act[1];
    childTwo.act[2] = parentOne.act[2];
    childTwo.act[3] = parentOne.act[3];
    childTwo.act[4] = parentOne.act[4];
    childTwo.act[5] = parentOne.act[5];

    //mutate
    int mutateChance = rand() % 100;
    int mutateActivity = rand() % 10;
    int mutateChoice = rand() % 3;
    int randIndex = 0;

    // mutate child one
    mutateChance = rand() % 100;

    if (mutateChance < 10) {
        mutateActivity = rand() % 10;
        mutateChoice = rand() % 3;
        randIndex = 0;

        // mutate room
        if (mutateChoice == 0) {
            randIndex = rand() % rooms.size();
            childOne.act[mutateActivity].room = rooms.at(randIndex).first;
            childOne.act[mutateActivity].roomCapacity = rooms.at(randIndex).second;
        }
        // mutate time
        if (mutateChoice == 1) {
            randIndex = rand() % times.size();
            childOne.act[mutateActivity].time = times.at(randIndex);
        }
        //mutate facilitator
        if (mutateChoice == 2) {
            randIndex = rand() % facilitators.size();
            childOne.act[mutateActivity].facilitator = facilitators.at(randIndex);
        }
        // recalculate fitness score of mutated activity
        childOne.act[mutateActivity].fitnessScore = 0;
        calculateFitnessOneActivity(childOne.act[mutateActivity]);
    };

    // mutate child two
    mutateChance = rand() % 100;

    if (mutateChance < 10) {
        mutateActivity = rand() % 10;
        mutateChoice = rand() % 3;
        randIndex = 0;

        // mutate room
        if (mutateChoice == 0) {
            randIndex = rand() % rooms.size();
            childTwo.act[mutateActivity].room = rooms.at(randIndex).first;
            childTwo.act[mutateActivity].roomCapacity = rooms.at(randIndex).second;
        }
        // mutate time
        if (mutateChoice == 1) {
            randIndex = rand() % times.size();
            childTwo.act[mutateActivity].time = times.at(randIndex);
        }
        //mutate facilitator
        if (mutateChoice == 2) {
            randIndex = rand() % facilitators.size();
            childTwo.act[mutateActivity].facilitator = facilitators.at(randIndex);
        }
        // recalculate fitness score of mutated activity
        childTwo.act[mutateActivity].fitnessScore = 0;
        calculateFitnessOneActivity(childTwo.act[mutateActivity]);
    };

    calculateFitnessAllActivities(childOne);
    calculateFitnessAllActivities(childTwo);
    minHeap.push(childOne);
    minHeap.push(childTwo);
}

int main(){

    srand(time(0));

    activity SLA100A;
    SLA100A.name = "SLA100A";
    SLA100A.expectedEnrollment = 50;
    SLA100A.preferredFacilitator = {"Glen", "Lock", "Banks", "Zeldin"};
    SLA100A.normalFacilitator = {"Numen", "Richards"};
    SLA100A.otherFacilitator = {"Shaw", "Singer", "Uther", "Tyler" };

    activity SLA100B;
    SLA100B.name = "SLA100B";
    SLA100B.expectedEnrollment = 50;
    SLA100B.preferredFacilitator = { "Glen", "Lock", "Banks", "Zeldin" };
    SLA100B.normalFacilitator = { "Numen", "Richards" };
    SLA100B.otherFacilitator = { "Shaw", "Singer", "Uther", "Tyler" };

    activity SLA191A;
    SLA191A.name = "SLA191A";
    SLA191A.expectedEnrollment = 50;
    SLA191A.preferredFacilitator = { "Glen", "Lock", "Banks", "Zeldin" };
    SLA191A.normalFacilitator = { "Numen", "Richards" };
    SLA191A.otherFacilitator = { "Shaw", "Singer", "Uther", "Tyler" };

    activity SLA191B;
    SLA191B.name = "SLA191B";
    SLA191B.expectedEnrollment = 50;
    SLA191B.preferredFacilitator = { "Glen", "Lock", "Banks", "Zeldin" };
    SLA191B.normalFacilitator = { "Numen", "Richards" };
    SLA191B.otherFacilitator = { "Shaw", "Singer", "Uther", "Tyler" };

    activity SLA201;
    SLA201.name = "SLA201";
    SLA201.expectedEnrollment = 50;
    SLA201.preferredFacilitator = { "Glen", "Banks", "Zeldin", "Shaw"};
    SLA201.normalFacilitator = { "Numen", "Richards", "Singer" };
    SLA201.otherFacilitator = { "Lock", "Uther", "Tyler" };

    activity SLA291;
    SLA291.name = "SLA291";
    SLA291.expectedEnrollment = 50;
    SLA291.preferredFacilitator = { "Lock", "Banks", "Zeldin", "Singer" };
    SLA291.normalFacilitator = { "Numen", "Richards", "Shaw", "Tyler"};
    SLA291.otherFacilitator = { "Glen", "Uther "};

    activity SLA303;
    SLA303.name = "SLA303";
    SLA303.expectedEnrollment = 60;
    SLA303.preferredFacilitator = { "Glen", "Zeldin", "Banks" };
    SLA303.normalFacilitator = { "Numen", "Singer", "Shaw" };
    SLA303.otherFacilitator = { "Lock", "Richards", "Uther", "Tyler" };

    activity SLA304;
    SLA304.name = "SLA304";
    SLA304.expectedEnrollment = 25;
    SLA304.preferredFacilitator = { "Glen", "Banks", "Tyler" };
    SLA304.normalFacilitator = { "Numen", "Singer", "Shaw", "Richards", "Uther", "Zeldin" };
    SLA304.otherFacilitator = { "Lock" };

    activity SLA394;
    SLA394.name = "SLA394";
    SLA394.expectedEnrollment = 20;
    SLA394.preferredFacilitator = { "Tyler", "Singer" };
    SLA394.normalFacilitator = { "Richards", "Zeldin" };
    SLA394.otherFacilitator = { "Lock", "Glen", "Banks", "Shaw", "Uther", "Numen" };

    activity SLA449;
    SLA449.name = "SLA449";
    SLA449.expectedEnrollment = 60;
    SLA449.preferredFacilitator = { "Tyler", "Singer", "Shaw" };
    SLA449.normalFacilitator = { "Zeldin", "Uther" };
    SLA449.otherFacilitator = { "Lock", "Glen", "Banks", "Richards", "Numen" };

    activity SLA451;
    SLA451.name = "SLA451";
    SLA451.expectedEnrollment = 100;
    SLA451.preferredFacilitator = { "Tyler", "Singer", "Shaw" };
    SLA451.normalFacilitator = { "Zeldin", "Uther", "Richards", "Banks" };
    SLA451.otherFacilitator = { "Lock", "Glen", "Numen" };

    // Min heap
    priority_queue<schedule, vector<schedule>, CompareMyObjects> minHeap;
    priority_queue<schedule, vector<schedule>, CompareMyObjects> minHeapTwo;

    // Populate heap
    populateHeap(SLA100A, SLA100B, SLA191A, SLA191B,
        SLA201, SLA291, SLA303, SLA304, SLA394,
        SLA449, SLA451, minHeap, numberOfPopulation);

    for (int j = 0; j < numberOfGenerations; j++) {
        for (int i = 0; i < numberOfCyclesForOneGeneration; i++) {
            makeChildren(minHeap, minHeapTwo);
        }
    }

    //output to file
    while (minHeap.size() > 1) {
        minHeap.pop();
    }
    schedule bestSched = minHeap.top();

    ofstream outfile("output.txt");

    outfile << "This is the best schedule with fitness score of: " + to_string(bestSched.fitnessScore) + "\n\n";
    for (int i = 0; i < bestSched.act.size(); i++) {
        outfile << bestSched.act[i].name + "\n";
        outfile << bestSched.act[i].room + "\n";
        outfile << bestSched.act[i].time + "\n";
        outfile << bestSched.act[i].facilitator + "\n";
        outfile << "Fitness Score: " + to_string(bestSched.act[i].fitnessScore) + "\n\n";
    }
    outfile.close();

}