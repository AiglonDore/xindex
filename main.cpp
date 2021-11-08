#include<iostream>
#include<string>
#include<fstream>
#include "core.h"

#define EXIT_WRONG_ARGC 1
#define EXIT_WRONG_FILE 2
#define EXIT_NOSTYLE 3
#define EXIT_NOFILE_SORT 4
#define EXIT_NOSORT_ALG 5
#define EXIT_WRONG_OPTIONS 6
#define EXIT_LOG 9

using namespace std;

/*
* @requires output is a valid and empty pointer
* @assigns *output with name+extension
* @ensures *output holds the full name of the file with the correct extension
*/
inline void buildFileName(string* output, char* name, string extension = ".aux")//Sert à générer le nom des fichiers
{
	int i(0);
	while (name[i] != '\0')
	{
		*output += name[i];
		i++;
	}
	output->append(extension);
}

int main(int argc, char *argv[])
{
	cout << "This is xindex, a new tool to call sorting programs!" << endl;
	bool verbose(false);
	if (argc != 2 && argc != 3)//Un seul argument: le nom du fichier sans extension
	{
		ofstream log("xindex.log");
		if (log)
		{
			log << "This is xindex, a new tool to call sorting programs!\n";
			log << "Analysing input sequence...\n";
			log << "Wrong argument number.\n";
			log << "One and only one argument is expected, or 2 if option -verbose is used.\n";
			log << "xindex exited with error code 1.\n";
			log << "End transmission.\n";
			cout << "Transcript written on xindex.log." << endl;
		}
		else
		{
			cerr << "Unable to write log file xindex.log" << endl;
		}
		cerr << "Wrong argument number." << endl;
		cerr << "One and only one argument is expected, or 2 if option -verbose is used." << endl;
		cerr << "xindex exited with error code " << 1 << "." << endl;
		log.close();
		return EXIT_WRONG_ARGC;
	}
	string* filename = nullptr;
	string* logname = nullptr;
	logname = new string;
	filename = new string;
	if (argc == 2)
	{
		buildFileName(filename, argv[1]);
		buildFileName(logname, argv[1], "_xindex.log");
	}
	else if (string(argv[1])==string("-verbose") || string(argv[1]) == string("-v"))
	{
		verbose = true;
		buildFileName(filename, argv[2]);
		buildFileName(logname, argv[2], "_xindex.log");
		cout << "Option verbose enabled." << endl;
	}
	else if (string(argv[2]) == string("-verbose") || string(argv[1]) == string("-v"))
	{
		verbose = true;
		buildFileName(filename, argv[1]);
		buildFileName(logname, argv[1], "_xindex.log");
		cout << "Option verbose enabled." << endl;
	}
	else
	{
		delete filename;
		filename = nullptr;
		delete logname;
		logname = nullptr;
		ofstream log("xindex.log");
		if (log)
		{
			log << "This is xindex, a new tool to call sorting programs!\n";
			log << "Analysing input sequence...\n";
			cerr << "Option not recognized." << endl;
			cerr << "xindex [-verbose] file" << endl;
			cerr << "xindex exited with error code " << 6 << "." << endl;
			log << "Option not recognized.\n";
			log << "xindex [-verbose] file\n";
			log << "xindex exited with error code 6.\n";
			log << "End transmission.\n";
			cout << "Transcript written on xindex.log." << endl;
		}
		else
		{
			cerr << "Unable to write log file xindex.log" << endl;
			cerr << "Option not recognized." << endl;
			cerr << "xindex [-verbose] file" << endl;
			cerr << "xindex exited with error code " << 6 << "." << endl;
		}
		log.close();
		return EXIT_WRONG_OPTIONS;
	}
	ifstream file(filename->c_str());//Ouverture du fichier aux en lecture
	ofstream log(logname->c_str());//Fichier de journal ouvert en écriture
	if (!log)
	{
		cerr << "Unable to write log file " << *logname << "." << endl;
		cerr << "Task aborted." << endl;
		delete filename;
		filename = nullptr;
		delete logname;
		logname = nullptr;
		log.close();
		file.close();
		cerr << "xindex exited with error code 9." << endl;
		return EXIT_LOG;
	}
	log << "This is xindex, a new tool to call sorting programs!\n";
	cout << "File " << *logname << " successfully opened!" << endl;
	log << "Analysing input sequence...\n";
	if (verbose)
	{
		log << "Option verbose enabled.\n";
		log << "More will be written in the terminal.\n";
	}
	if (file)
	{
		cout << "Opening file " << *filename << endl;
		log << "Opening file " << *filename << "\n";
		try
		{
			log << "File " << *filename << " is valid. Proceeding to process file.\n";
			processFile(&file,filename,verbose,&log);
		}
		catch (int e)
		{
			log << "Errors have been encountered: code " << e << "\n";
			switch (e)
			{
			case 3:
				cerr << "No style detected in " << *filename << endl;
				cerr << "Closing file " << *filename << ". Task aborted." << endl;
				cerr << "xindex exited with error code " << 3 << "." << endl;
				log << "No style header has been found while looking in " << *filename << ".\n";
				log << "Closing file " << *filename << ". Task aborted.\n";
				log << "xindex exited with error code 3.\n";
				log << "End transmission.\n";
				file.close();
				log.close();
				cout << "Transcript has been written to " << *logname << "." << endl;
				delete filename;
				filename = nullptr;
				delete logname;
				logname = nullptr;
				return EXIT_NOSTYLE;
			case 4:
				cerr << "No file to sort detected in " << *filename << endl;
				cerr << "Closing file " << *filename << ". Task aborted." << endl;
				cerr << "xindex exited with error code " << 4 << "." << endl;
				log << "No file extensions for sorting have been found while looking in " << *filename << ".\n";
				log << "Closing file " << *filename << ". Task aborted.\n";
				log << "xindex exited with error code 4.\n";
				log << "End transmission.\n";
				file.close();
				delete filename;
				log.close();
				filename = nullptr;
				cout << "Transcript has been written to " << *logname << "." << endl;
				delete logname;
				logname = nullptr;
				return EXIT_NOFILE_SORT;
			case 5:
				cerr << "No sort program detected in " << *filename << endl;
				cerr << "Closing file " << *filename << ". Task aborted." << endl;
				cerr << "xindex exited with error code " << 5 << "." << endl;
				log << "No sort program has been found while looking in " << *filename << ".\n";
				log << "Closing file " << *filename << ". Task aborted.\n";
				log << "xindex exited with error code 5.\n";
				log << "End transmission.\n";
				file.close();
				log.close();
				delete filename;
				filename = nullptr;
				cout << "Transcript has been written to " << *logname << "." << endl;
				delete logname;
				logname = nullptr;
				return EXIT_NOSORT_ALG;
			}
		}
		cout << "Closing file " << *filename << "." << endl;
		cout << "Sort completed." << endl;
		log << "Closing file " << *filename << ".\n";
		log << "Sort has sucessfully completed.\n";
		if (verbose)
		{
			log << "Sort logs are available in this file.\n";
		}
		else
		{
			log << "If you want to have sort logs in this file, please use option -verbose.\n";
		}
		log << "End transmission.\n";
		cout << "Transcript has been written to " << *logname << "." << endl;
		file.close();
		log.close();
		delete filename;
		filename = nullptr;
		delete logname;
		logname = nullptr;
		return EXIT_SUCCESS;
	}
	else
	{
		file.close();
		cerr << "File " << *filename << " is not valid." << endl;
		cerr << "xindex exited with error code " << 2 << "." << endl;
		log << "File " << *filename << " is not valid.\n";
		log << "Please check the name of the file and its directory.\n";
		log << "Then, rerun xindex.\n";
		log << "xindex exited with error code " << 2 << ".\n";
		log << "End transmission.\n";
		delete filename;
		filename = nullptr;
		log.close();
		cout << "Transcript has been written to " << *logname << "." << endl;
		delete logname;
		logname = nullptr;
		return EXIT_WRONG_FILE;
	}
}