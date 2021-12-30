#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED
#include <string>
#include <fstream>
/*
* @requires nothing
* @assigns process string as a c char array and calls system with it
* @ensures return code from system() call
*/
inline auto system(std::string const &input) { return system(input.c_str()); };
inline auto system(std::string* const input) { return system(input->c_str()); };

inline std::string purename(std::string const& name, std::string extension = ".aux");
void processFile(std::ifstream* file, std::string* filename, bool verbose, std::ofstream* log, bool *gloss);

#endif // !CORE_H_INCLUDED

