/*
 * main.cpp
 *
 *	Created on: 3 Nov. 2017
 *		Author: Brandon
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <fstream>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#define SRC "cpp"
#define HEADER "h"
#define MAX_STR_LEN 4096 // at least greater than 260 (windows limit)
#define MAX_COMMENT_LEN 10000 // arbitrary

// NOTE(brandon) Add moe types of source files
const int EXT_COUNT = 4;
const char SRC_EXTS[EXT_COUNT][MAX_STR_LEN] = {
	"cpp",
	"c",
	"java", 
	"kt"
};

bool IsSourceFile(
		char* src)
{
	char newStr[MAX_STR_LEN]; strcpy(newStr, src);
	char* tok = strtok(newStr, ".");
	while (tok != NULL) {
		// c++ header or source file
		for (int i = 0; i < EXT_COUNT; i++) {
			if (strcmp(tok, SRC_EXTS[i]) == 0) {
				return true;
			}
		}
		tok = strtok(NULL, ".");
	}
	return false;
}

// NOTE(brandon) Sort by file and by name
// NOTE(brandon) Create different types of notes and categories
void RecursiveFind(
		char* src,
		queue<string>* queue,
		vector<string>* files)
{
	string current = queue->front();
	queue->pop();
	DIR* dir;
	dirent* ent;

	if ((dir = opendir (current.c_str())) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			// get info to check if file or directory
			char newStr[MAX_STR_LEN] = "";
			strcat(newStr, current.c_str());
			strcat(newStr, "\\");
			strcat(newStr, ent->d_name);

			// ignore current and parent directory and make
			// sure it's a directory
			struct stat statPath;
			stat(newStr, &statPath);
			if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0
					&& ent->d_name[0] != '.' // no hidden dirs
					&& S_ISDIR(statPath.st_mode)) {
				queue->push(string(newStr));
				RecursiveFind(src, queue, files);
			} else if (S_ISREG(statPath.st_mode) && IsSourceFile(newStr)) { // file
				// add to list
				files->push_back(string(newStr));
			}
		}
		closedir (dir);
	} else {
		return;
	}
}

void GetSourceFiles(
		char* directory, 
		vector<string>* files)
{
	queue<string> queue;
	queue.push(string(directory));
	RecursiveFind(directory, &queue, files);
}

void Parse(
		vector<string> files,
		char* outputFile)
{
	ifstream input;

	// create the output file
	ofstream output;
	output.open(outputFile);

	if (output.bad()) {
		cout << "Error: Bad output file" << endl;
		exit(EXIT_FAILURE);
	}

	// loop through each files contents and check if we are in a comment or not
	// and if we are and there's the correct documentation info then add
	// the contents of this documents to the output file
	// doc comments are in the form NOTE(name) test
	for (int f = 0; f < files.size(); f++) {
		input.open(files.at(f));
		
		if (input.bad()) {
			cout << "Could not open input file: " << files.at(f) << endl;
			exit(EXIT_FAILURE);
		}
		
		int lineCount = 0;
		bool hasNotes = false;
		while (!input.eof() && !input.bad()) {
			lineCount++;
			char bufLine[MAX_STR_LEN];
			char bufName[MAX_STR_LEN];
			char bufCont[MAX_STR_LEN];
			int ncount = 0, count = 0, ccount = 0;
			
			input.getline(bufLine, MAX_STR_LEN);
			for (int i = 0; i < strlen(bufLine); i++) {
				int c = bufLine[i];

				if (c == EOF) break;

				if (i + 4 < strlen(bufLine)) {
					if (bufLine[i] == 'N' && bufLine[i+1] == 'O' && bufLine[i+2] == 'T' && bufLine[i+3] == 'E' && bufLine[i+4] == '(') {				
						// read the name
						i += 5;
						while (bufLine[i] != ')') {
							bufName[ncount++] = bufLine[i++];
						} i += 2; bufName[ncount] = '\0';

						// read the rest of the buffer
						while (i < strlen(bufLine)) { // strange but works :)
							bufCont[ccount++] = bufLine[i++];
						} bufCont[ccount] = '\0';
						hasNotes = true;
						cout << "Line: " << lineCount << " [" << bufName << "]: " << bufCont << endl;
						output << "Line: " << lineCount << " [" << bufName << "]: " << bufCont << endl;
					}
				}
			}
		}

		if (hasNotes) {
			output << "\tIn File: " << files.at(f) << endl;
			cout << "\tIn File: " << files.at(f) << endl << endl;
		}

		input.close();
	}
}

int main(
		int argc, 
		char* argv[])
{
	if (argc != 3) {
		cout << "Wrong number of arguments." << endl;
		exit(EXIT_FAILURE);
	}

	cout << argv[1] << " in to " << argv[2] << endl;
	cout << "Checking validity..." << endl;

	// open the directory givn as an argument and check it's a valid file
	// either a file or directory
	DIR* dir;
	
	if ((dir = opendir(argv[1])) == NULL) {
		cout << "Argument 1 given was not a valid file." << endl;
		exit(EXIT_FAILURE);
	}

	closedir(dir);

	cout << "Checking if valid arguments..." << endl;

	// check if the given directory is not only valid but actually
	// a directory
	struct stat stat_s;
	stat(argv[1], &stat_s);
	
	if (!S_ISDIR(stat_s.st_mode)) {
		cout << "Argument 1 given was not a directory." << endl;
		exit(EXIT_FAILURE);
	}
   
	cout << "Searching..." << endl;

	// get the list of all source files in the directory given
	// as an argument
	vector<string> files;
	GetSourceFiles(argv[1], &files);
	
	cout << "Parsing files..." << endl;
	Parse(files, argv[2]);

	return EXIT_SUCCESS;
}


