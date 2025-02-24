#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string removeQuotes(string path); // function prototype for quote removal.

int main()
{
    const string commanderMarker = " \"event\":\"Commander\""; // constant string for the Commander event in the journal. this will give the CMDR name.
    const string progressMarker  = " \"event\":\"Progress\"";  // constant string for the progress event in the journal
    const string rankMarker      = " \"event\":\"Rank\"";      // constant string for the rank event in the journal

    const int vals = 8; // declare a constant integer for the size of all of the relevant arrays. There are eight careers (6 careers + 2 naval ranks), 
                        // so the arrays should all have size 8.
    // there are 8 careers in Elite Dangerous. The following array lists all of them:
    const string careers[vals] = { "Combat", "Trade", "Exploration", "Mercenary", "Exobiology", "the Imperial Navy", "the Federal Navy", "CQC" };
    // create and initialize arrays with all of the various rank names in each career, from nothing to Elite V:
    const int maxCareerRankNum = 13; // all careers have 14 ranks, from nothing (0) up to Elite V (13)
    const int maxNavalRankNum = 14;  // the naval ranks actually have 15, from 0 (none) to 14 (King/Admiral)
    const string ranksMatrix[8][15] = {
                                 { "Harmless",       "Mostly Harmless",      "Novice",   "Competent",         "Expert",        "Master",              "Dangerous",       "Deadly",     "Elite",      "Elite I",              "Elite II",       "Elite III",    "Elite IV",     "Elite V" } ,
                                 { "Penniless",      "Mostly Penniless",     "Peddler",  "Dealer",            "Merchant",      "Broker",              "Entrepreneur",    "Tycoon",     "Elite",      "Elite I",              "Elite II",       "Elite III",    "Elite IV",     "Elite V" },
                                 { "Aimless",        "Mostly Aimless",       "Scout",    "Surveyor",          "Trailblazer",   "Pathfinder",          "Ranger",          "Pioneer",    "Elite",      "Elite I",              "Elite II",       "Elite III",    "Elite IV",     "Elite V" },
                                 { "Defenceless",    "Mostly Defenceless",   "Rookie",   "Soldier",           "Gunslinger",    "Warrior",             "Gladiator",       "Deadeye",    "Elite",      "Elite I",              "Elite II",       "Elite III",    "Elite IV",     "Elite V" },
                                 { "Directionless",  "Mostly Directionless", "Compiler", "Collector",         "Cataloguer",    "Taxonomist",          "Ecologist",       "Geneticist", "Elite",      "Elite I",              "Elite II",       "Elite III",    "Elite IV",     "Elite V" },
                                 { "None",           "Outsider",             "Serf",     "Master",            "Squire",        "Knight",              "Lord",            "Baron",      "Viscount",   "Count",                "Earl",           "Marquis",      "Duke",         "Prince",       "King" },
                                 { "None",           "Recruit",              "Cadet",    "Midshipman",        "Petty Officer", "Chief Petty Officer", "Warrant Officer", "Ensign",     "Lieutenant", "Lieutenant Commander", "Post Commander", "Post Captain", "Rear Admiral", "Vice Admiral", "Admiral" },
                                 { "Helpless",       "Mostly Helpless",      "Amateur",  "Semi Professional", "Professional",  "Champion",            "Hero",            "Legend",     "Elite",      "Elite I",              "Elite II",       "Elite III",    "Elite IV",     "Elite V" }
    };
    // prompt the user to input the path to their journal file
    cout << "Enter the full path for the journal file you'd like to look at.\n";
    cout << "These will be found in C:\\Users\\(your name)\\Saved Games\\Frontier Developments\\Elite Dangerous.\n";
    cout << "Right-click the most recent journal file (Journal.YYYY-MM-DDTXXXXXXXX.01.log), then click \"Copy as path\".\n";
    cout << "Paste that here:\n";
    // testing journal files can be found in a subfolder in my journal folder:
    // C:\Users\Lily\Saved Games\Frontier Developments\Elite Dangerous\test cases

    string journalPath; // declare a string for the path to the journal file 
    getline(cin, journalPath); // get the whole line of input, store to journalPath

    // is there a quote at the beginning? let's remove it and extract the filename without quotes if so:
    journalPath = removeQuotes(journalPath);


    // create the input filestream and open it at the user-specified path:
    ifstream journal; // declare an ifstream object, call it journal
    journal.open(journalPath); // attempt to open the journal file with the file path specified in the journalPath variable

    // if there's an error reading the journal file, stop, ask the user to try again until the journal can be read:
    while (!journal) {
        system("cls"); // clear the console window
        cout << "The journal file could not be found at the path you gave. Double-check your path and enter it again:" << endl;
        getline(cin, journalPath);
        journalPath = removeQuotes(journalPath);
        journal.open(journalPath); // attempt to open the journal file with the file path specified in the journalPath variable
    }
    system("cls"); // clear the console window

    cout << "Journal successfully located and loaded. Let's begin reading it!\n\n";
    string input; // declare a string variable to read journal input

/*================================================================================================================================================================================*/
    /* get input up to the Commander marker and process it */
    int numberOfCMDREvents = 0; // debug variable for number of CMDR events 
    char checkAgain = 'Y'; // variable to store whether the user wants to continue or not
    long currentPosition = 0L;
    string cmdrName = "a value that should ideally never be seen";
    long lastGoodPosition = 0L;
    while (toupper(checkAgain) == 'Y') {
        journal.seekg(lastGoodPosition, ios::beg); // go back to the last good position - beginning of the file at first. 
                                                   // this value gets updated later to 100 bytes behind the most recent CMDR name.

        //cout << "current position = " << currentPosition << endl;
        // catch any infinite loop that may arise from no CMDR name:
        while (input != commanderMarker)
        {
            // read each journal string up to a comma to the input variable until we reach the CMDR name:
            getline(journal, input, ',');

            // however, a successfully opened journal file with no CMDR name will cause the program to enter an infinite loop.
            // this could be caused by the user putting in their journal file too early.
            // journal files get created when the game opens, but the CMDR name isn't filled until the user logs into Open, PG, or Solo.
            // if we reach the end of the file before input == commanderMarker, then something's not right. we need to exit if that happens.
            if (journal.eof()) {
                //cout << "current position = " << currentPosition << endl;
                cout << "ERROR: The file was opened successfully, but the program could not find your name." << endl;
                cout << "Double-check your file path. If you're sure the path is correct, check that the journal isn't blank, or that it was loaded too early." << endl;
                cout << "Just to be safe, if you're using a new journal file, wait until you're in your ship before using this tool." << endl;
                cout << "Here's the file path you entered: " << "\n" << journalPath << endl;
                cout << "Press enter to exit the program.";
                cin.get();
                return -1; // exit the program if this case happens.
            }
        }
        numberOfCMDREvents++; // increment the number of CMDR events
        // if there's another rank progress value, aka after a relog, there will be another instance of the CMDR name that could get skipped.
        // let's see if we can find that, if there is one.
        // store the current read position. we'll come back to this spot later if no more instances of the CMDR name exist.
        // otherwise, we'll update this position if there is another instance.
        currentPosition = journal.tellg();

        // now that we have our current read position to fall back to, 
        // let's keep reading and see if there are any more instances of the CMDR name:
        getline(journal, input, ','); // read forward just to make input not equal to commanderMarker
        while (input != commanderMarker)
        {
            //cout << "current position = " << journal.tellg() << endl;

            // read each journal string up to a comma to the input variable until we reach the CMDR name:
            getline(journal, input, ',');
            // if we find another CMDR event:
            if (input == commanderMarker) {
                numberOfCMDREvents++; // increment the number of CMDR events
                currentPosition = journal.tellg(); // update the return position
                //cout << "CMDR Name " << numberOfCMDREvents << " found at position " << currentPosition << endl; // debug
                getline(journal, input, ','); // read forward once to keep the loop going
            }
            // if we've reached the end of the file, break out of the loop
            if (journal.eof()) {
                //cout << "No more instances of CMDR name found." << endl;
                journal.seekg(currentPosition, ios::beg);
                //cout << "Last CMDR Name @ " << currentPosition << "; name #" << numberOfCMDREvents << endl;
                break; // break out of the loop if the end of the file is reached and no more CMDR events are found
            }
        }

        lastGoodPosition = currentPosition - 100L; // set the last good position to be our current position minus 100 bytes to go back to if the user types 'Y' but nothing changes
        //cout << "Number of CMDR events found = " << numberOfCMDREvents << endl;
        // now that we're at the commander name section, we need to get the CMDR name.
        getline(journal, input, ',');  // skip past the FID, we don't need that.
        getline(journal, input, '}');  // here we go. *now* we have the name.
        //cout << input << endl << endl; // output for testing
        string rawNameInput = input;   // capture this value and save it
        int rawNameLength = int(rawNameInput.length());
        //cout << "length = " << rawNameLength << endl;
        int rawNameColonPos = int(rawNameInput.find(':'));
        int cmdrNameLength = rawNameLength - 9 - 2;
        // the string starts with a set of 9 characters ("Name":"), then the name, then 2 at the end (" ).
        // taking the full string length, minus 11 characters, should give the length of the CMDR's name.
        cmdrName = rawNameInput.substr((rawNameColonPos + 2), cmdrNameLength);
        cout << "Greetings, CMDR " << cmdrName << "!" << endl << endl;

        /*================================================================================================================================================================================*/
            /* get input up to the Rank marker and process it */
        while (input != rankMarker)
        {
            // read each journal string up to a comma to the input variable until we reach the rank progress numbers:
            //cout << "Current read position = " << journal.tellg() << endl;
            getline(journal, input, ',');
            if (journal.eof()) {
                cout << "ERROR: The file was opened successfully, and your CMDR name was found, but the program could not find your ranks." << endl;
                cout << "Double-check your file path. If you're sure the path is correct, check that the journal isn't blank, or that it was loaded too early." << endl;
                cout << "Just to be safe, if you're using a new journal file, wait until you're in your ship before using this tool." << endl;
                cout << "Here's the file path you entered: " << "\n" << journalPath << endl;
                cout << "Press Enter to exit the program.";
                cin.get();
                return -2; // exit the program if this case happens.
            }
        }
        /* getting the numerical rank values */
        string rankValues[vals];
        // get the first 7 rank values. there are 8, in this order: Combat, Trade, Explore, Soldier, Exobiologist, Empire, Federation, CQC.
        // we'll handle CQC separately since it doesn't have a comma after it. we'll read up to the closing brace.
        for (int i = 0; i < (vals - 1); i++)
        {
            getline(journal, rankValues[i], ',');
        }
        getline(journal, rankValues[7], '}');
        // create arrays to hold all relevant values:
        int rankStringLengths[vals];  // array for the length of each string
        size_t rankColonPositions[vals]; // array to hold the colon position in each string
        int rankValueLengths[vals];   // array to hold the calculated rank value length (should only be 1 or 2)
        string rawRankNumbers[vals];  // array to hold the raw string 
        const char* rawRanks[vals];   // array of c-strings to hold conversions from string to c-string, enabling conversions to integers.
        int rankValue[vals];          // array for the output percentages
        bool atMaxRank[vals];         // array of booleans to check if the CMDR is at max rank. will change output message if true.
        // initialize all arrays:
        for (int i = 0; i < vals; i++) {
            rankStringLengths[i] = -1;
            rankColonPositions[i] = -1;
            rankValueLengths[i] = -1;
            rawRankNumbers[i] = "-1";
            rawRanks[i] = "- 1";
            rankValue[i] = -1;
            atMaxRank[i] = false;
        }
        // extract rank numbers:
        for (int i = 0; i < vals; i++) {
            rankStringLengths[i] = int(rankValues[i].length()); // get the length of the string in the rankValues array at position i
            rankColonPositions[i] = int(rankValues[i].find(':')); // find the numerical position of the colon and record that to the rankColonPositions array in position i
            rawRankNumbers[i] = rankValues[i].substr((rankColonPositions[i] + 1), rankValueLengths[i]); // starting 1 character ahead of the colon, copy a substring of length 
            // rankValueLengths into the rawrankNumbers array.
            rawRanks[i] = rawRankNumbers[i].c_str(); // convert this substring to a c-string
            rankValue[i] = atoi(rawRanks[i]); // atoi converts a c-string to an integer
            //cout << "In " << careers[i] << ", you are rank " << rankValue[i] << " (raw), which corresponds to " << ranksMatrix[i][rankValue[i]] << endl;
        }

        // check if the ranks up to the naval ranks are maxed out:
        for (int i = 0; i < (vals - 3); i++) {
            if (rankValue[i] == maxCareerRankNum)
            {
                atMaxRank[i] = true;
            }
            else
            {
                atMaxRank[i] = false;
            }
        }
        // check if the naval ranks are maxed:
        for (int i = (vals - 3); i < vals; i++) {
            if (rankValue[i] == maxNavalRankNum)
            {
                atMaxRank[i] = true;
            }
            else
            {
                atMaxRank[i] = false;
            }
        }
        // check if the CQC rank is maxed out (in the real world this should never be true)
        if (rankValue[(vals - 1)] == maxCareerRankNum)
        {
            atMaxRank[(vals - 1)] = true;
        }
        else
        {
            atMaxRank[(vals - 1)] = false;
        }

        /*================================================================================================================================================================================*/
            /* get input up to the progress marker and process it */
        getline(journal, input, ','); // read the journal file up to a comma
        while (input != progressMarker)
        {
            // read each journal string to a new line until we reach the rank progress numbers:
            //cout << input;
            getline(journal, input, ',');
            if (journal.eof()) {
                cout << "ERROR: The file was opened successfully, your CMDR name was found, and the ranks were found,\n"
                    << "but the program could not find your progress values." << endl;
                cout << "Double-check your file path. If you're sure the path is correct, check that the journal isn't blank, or that it was loaded too early." << endl;
                cout << "Just to be safe, if you're using a new journal file, wait until you're in your ship before using this tool." << endl;
                cout << "Here's the file path you entered: " << "\n" << journalPath << endl;
                cout << "Press Enter to exit the program.";
                cin.get();
                return -3; // exit the program if this case happens.
            }
        }

        /* getting the progress values */
        string progressValues[8];
        // get the first 7 progress values. there are 8, in this order: Combat, Trade, Explore, Soldier, Exobiologist, Empire, Federation, CQC.
        // we'll handle CQC separately since it doesn't have a comma after it.
        for (int i = 0; i < 7; i++)
        {
            getline(journal, progressValues[i], ',');
            //cout << progressValues[i] << endl;
        }
        getline(journal, progressValues[7], '}');
        //cout << progressValues[7] << endl;

        // create arrays to hold all relevant values:
        int progressStringLengths[vals];  // array for the length of each string
        size_t progressColonPositions[vals]; // array to hold the colon position in each string
        int progressValueLengths[vals];   // array to hold the calculated progress value length (should only be 1 or 2)
        string rawProgressNumbers[vals];  // array to hold the raw string 
        const char* percentage[vals];     // array of c-strings to hold conversions from string to c-string, enabling conversions to integers.
        int progressPercent[vals];        // array for the output percentages
        // initialize all arrays:
        for (int i = 0; i < vals; i++) {
            progressStringLengths[i] = -1;
            progressColonPositions[i] = -1;
            progressValueLengths[i] = -1;
            rawProgressNumbers[i] = "-1";
            percentage[i] = "-1";
            progressPercent[i] = -1;
        }
        // begin populating the arrays with their relevant values,
        // ultimately extracting a numerical percentage from the raw journal file 
        for (int i = 0; i < vals; i++) {
            progressStringLengths[i] = int(progressValues[i].length()); // get the length of the string in the progressValues array at position i
            progressColonPositions[i] = int(progressValues[i].find(':')); // find the numerical position of the colon and record that to the progressColonPositions array in position i
            rawProgressNumbers[i] = progressValues[i].substr((progressColonPositions[i] + 1), progressValueLengths[i]); // starting 1 character ahead of the colon, copy a substring of length 
            // progressValueLengths into the rawProgressNumbers array.
            percentage[i] = rawProgressNumbers[i].c_str(); // convert this substring to a c-string
            progressPercent[i] = atoi(percentage[i]); // atoi converts a c-string to an integer

            // is the CMDR at max rank for a particular career? congratulate them if so, and don't display progress towards next rank, because there is no next rank!
            if (atMaxRank[i] == true)
            {
                cout << "In " << careers[i] << ", you are " << ranksMatrix[i][rankValue[i]] << ". Congratulations!" << endl;
            }
            else
            {
                cout << "In " << careers[i] << ", you are " << progressPercent[i] << "% of the way from " << ranksMatrix[i][rankValue[i]] << " to " << ranksMatrix[i][rankValue[i] + 1] << "." << endl;
            }
        }
        cout << "\nWould you like to check for more rank progress updates?" << endl;
        cout << "Enter Y if you'd like to check, or N if you don't: ";
        cin >> checkAgain;
        if (toupper(checkAgain) == 'Y') {
            journal.close();
            journal.open(journalPath);
            system("cls");
        }
    }
    cout << "\no7, CMDR " << cmdrName << "." << endl; 
    cout << "Press Enter to exit the program." << endl;
    cin.get(); // clear any lingering \n's out of the input buffer
    cin.get();
    return 0;
}

string removeQuotes(string path) {
    // is the first character of the passed string a quotation mark? if so, then we'll make a new string without the first and last characters:
    
    //cout << "Hello from the removeQuotes function. the first character of the passed string is: " << path[0] << endl;
    if (path[0] == '\"') {
        size_t inLen = path.length(); // find the length of this filename
        //cout << "len = " << inLen << endl;
        string noQuotes = path.substr(1, (inLen - 2));
        //cout << "returning: " << noQuotes << endl;
        return noQuotes;
    }
    else {
        //cout << "Hello from the removeQuotes function. No quotes here." << endl;
        string out = path;
        return out; // if the first character isn't a quote, then just return the string as given
    }
}