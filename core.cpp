#include <string>
#include<iostream>
#include<vector>
#include<fstream>
#include "core.h"
#include <cstdlib>

using namespace std;

string const sortMkI = "index@sort{makeindex}";
string const sortXdy = "index@sort{xindy}";
string const styleLine = "index@style";
string const newInd = "aux@new@index";

/*
* @requires nothing
* @assigns tmp
* @ensures tmp = string \ spaces
*/
inline string trimSpaces(string const& input, bool nobreakspace = false)
{
	string tmp = "";
	for (int i = 0; i < input.size(); i++)
	{
		if (!((input[i] == ' ')||(nobreakspace && input[i] == ' ')))//Le premier est une espace normale, le second est une espace insécable
		{
			tmp.push_back(input[i]);
		}
	}
	return tmp;
}

/*
* @requires extension is in name
* @assigns tmp
* @ensures tmp = name \ extension
*/
inline string purename(string const& name, string extension)
{
	string tmp("");
	size_t up = name.find(extension);
	for (size_t i = 0; i < up; i++)
	{
		tmp.push_back(name[i]);
	}
	return tmp;
}

/*
* @requires *file holds the file to read, *log holds the log file where log data will be written
* @assigns log file and calls makeindex or xindy
* @ensures if no exception, nothing, else exception code relative to a missing sorting alg, style or data file
*/
void processFile(std::ifstream* file, string* filename, bool verbose, ofstream* log, bool *gloss)
{
	string const pureName = purename(*filename);
	*log << "Sequences needed:\n";
	*log << "-" << sortMkI << " or " << sortXdy << ".\n";
	*log << "-" << styleLine << ".\n";
	*log << "-" << newInd << ".\n";
	bool styleFound(false), fileFound(false), algFound(false);
	bool xindy(false);
	bool init(false);//Sert à éviter de prendre en compte la définition des balises dans le .aux
	string currentStyle = "";
	string ligne;
	vector<vector<string>> index;//Colonnes: idx, ind, style
	int countLigne(0);
	while (getline(*file,ligne))
	{
		countLigne++;
		ligne = trimSpaces(ligne);
		if (init && !algFound && ligne.find(sortMkI) != string::npos)//Alg: makeindex
		{
			algFound = true;
			*log << "I found a sort algorithm: \"makeindex\".\n";
			*log << "It was at line " << countLigne << " in the aux file.\n";
			if (verbose)
			{
				cout << "Sort algorithm: \"makeindex\"." << endl;
			}
		}
		else if (init && !algFound && ligne.find(sortXdy) != string::npos)//Alg: xindy
		{
			algFound = true;
			xindy = true;
			*log << "I found a sort algorithm: \"xindy\".\n";
			*log << "It was at line " << countLigne << " in the aux file.\n";
			if (verbose)
			{
				cout << "Sort algorithm: \"xindy\"." << endl;
			}
		}
		else if (init && ligne.find(styleLine) != string::npos)//Ligne de style trouvée
		{
			styleFound = true;
			currentStyle = "";
			for (auto i = styleLine.size() + 2; i < ligne.size(); i++)
			{
				if (ligne[i] == '}')
				{
					break;
				}
				currentStyle.push_back(ligne[i]);
			}
			if (xindy)
			{
				currentStyle.append(".xdy");
			}
			else
			{
				currentStyle.append(".ist");
			}
			*log << "Style found: \"" << currentStyle << "\".\n";
			*log << "It will be used for the coming index.\n";
			*log << "It was at line " << countLigne << " in the aux file.\n";
			if (verbose)
			{
				cout << "Style found: \"" << currentStyle << "\"." << endl;
			}
		}
		else if (init && ligne.find(newInd) != string::npos)//Ligne de nouvel index trouvé
		{
			fileFound = true;
			*log << "New index definition found.\n";
			*log << "It was at line " << countLigne << " in the aux file.\n";
			vector<string> tmp = { "","",currentStyle };
			auto i = newInd.size() + 2;
			for (i; i < ligne.size(); i++)
			{
				if (ligne[i] == '}')
				{
					break;
				}
				tmp[0].push_back(ligne[i]);
			}
			i += 2;
			for (i; i < ligne.size(); i++)
			{
				if (ligne[i] == '}')
				{
					break;
				}
				tmp[1].push_back(ligne[i]);
			}
			*log << "Index original file extension: \"" << tmp[0] << "\"." << endl;
			*log << "Index destination file extension: \"" << tmp[1] << "\"." << endl;
			*log << "Style for this index: \"" << tmp[2] << "\"." << endl;
			if (verbose)
			{
				cout << "Index found." << endl;
				cout << "Index original file extension: \"" << tmp[0] << "\"." << endl;
				cout << "Index destination file extension: \"" << tmp[1] << "\"." << endl;
				cout << "Style for this index: \"" << tmp[2] << "\"." << endl;
			}
			index.push_back(tmp);
		}
		else if (ligne.find("start@document") != string::npos)//Arrêt: aucune donnée dans le corps du document
		{
			if (!init)
			{
				init = true;
			}
			else
			{
				*log << "Document starts here. No need to continue exploring the aux file.\n";
				*log << "Exploration ended at line " << countLigne << ".\n";
				break;
			}
		}
		if (!(*gloss) && ligne.find("newglossary") != string::npos)
		{
			*gloss = true;
		}
	}
	if (!styleFound)//Pas de style trouvé
	{
		throw 3;
	}
	if (!fileFound)//Pas de fichier trouvé
	{
		throw 4;
	}
	if (!algFound)//Pas d'alorithme trouvé
	{
		throw 5;
	}
	*log << index.size() << " index found." << endl;
	*log << "Original|destination|style." << endl;
	if (verbose)
	{
		cout << index.size() << " index found." << endl;
		cout << "Original|destination|style." << endl;
	}
	for (int i = 0; i < index.size(); i++)
	{
		*log << index[i][0] << "|" << index[i][1] << "|" << index[i][2] << "." << endl;
		if (verbose)
		{
			cout << index[i][0] << "|" << index[i][1] << "|" << index[i][2] << "." << endl;
		}
	}
	*log << "I'm going to call the sorting program now.\n";
	if (xindy)
	{
		for (int i = 0; i < index.size(); i++)
		{
			string tmp = "xindy -C utf8 -I latex -M \"%INDEX%\"\\";
			tmp.append(index[i][2]);
			tmp.append(" -L ");
			tmp.append(purename(index[i][2], ".xdy"));
			tmp.append(" -t ");
			tmp.append(pureName);
			if (i == 0)
			{
				tmp.append(".ilg");
			}
			else
			{
				tmp.append(".ilg");
				tmp.append(to_string(i));
			}
			tmp.append(" -o " + pureName + index[i][1]);
			tmp.push_back(' ');
			tmp.append(index[i][0]);
			*log << "Index " << i << ".\n";
			*log << "Executing command: " << tmp << ".\n";
			if (verbose)
			{
				cout << "Index " << i << ".\n";
				cout << "Executing command: " << tmp << ".\n";
			}
			system(tmp);
		}
	}
	else
	{
		for (int i = 0; i < index.size(); i++)
		{
			*log << "Index " << i + 1 << ".\n";
			string tmp = "makeindex -s \"%INDEX%\"\\" + index[i][2];
			tmp.append(" -t ");
			tmp.append(pureName);
			if (i == 0)
			{
				tmp.append(".ilg");
			}
			else
			{
				tmp.append(".ilg");
				tmp.append(to_string(i));
			}
			tmp.append(" -o ");
			tmp.append(pureName + string(".") + index[i][1] + " ");
			tmp.append(pureName + string(".") + index[i][0]);
			if (index[i][2].find("allemand") != string::npos)
			{
				tmp.append(" -g");
			}
			*log << "Executing command: " << tmp << ".\n";
			if (verbose)
			{
				cout << "Index " << i + 1 << ".\n";
				cout << "Executing command: " << tmp << ".\n";
			}
			int returnSys = system(tmp);
			if (returnSys != 0)
			{
				cerr << "Failed to process index " << i + 1 << "." << endl;
				cerr << "Check log file for more information." << endl;
				if (verbose) { cerr << "Error code: " << returnSys << "." << endl; }
				*log << "Failed to process index " << i + 1 << "." << endl;
				*log << "It failed with code " << returnSys << ".\n";
			}
			else
			{
				cout << "Sorting run of index " << i + 1 << " sucessfully completed." << endl;
				*log << "Sorting run of index " << i + 1 << " sucessfully completed." << endl;
			}
		}
	}
	cout << "Outputs and logs written. Check logs file for any problem." << endl;
	cout << "Logs are numbered the same order index were defined." << endl;
	*log << "Outputs and logs written. Check logs file for any problem." << endl;
	*log << "Logs are numbered the same order index were defined." << endl;
	if (verbose)
	{
		*log << "Sort logs are also available below!\n";
		string sortLogname = pureName;
		sortLogname.append(".ilg");
		for (int i = 0; i < index.size(); i++)
		{
			*log << "Log of sorting run " << i + 1 << ".\n";
			if (i != 0)
			{
				sortLogname.append(to_string(i));
			}
			ifstream sortLog(sortLogname.c_str());
			string ligneLog;
			while (getline(sortLog,ligneLog))
			{
				*log << "    -" << ligneLog << "\n";
			}
			sortLog.close();
			*log << "End of sorting run log " << i + 1 << ".\n";
		}
	}
}
