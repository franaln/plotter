/** @file base.h
    @brief Common definitions and fns */

#ifndef BASE_H
#define BASE_H

#include <iostream>

// colors
#define GRAY    "\033[90m"
#define RED     "\033[91m"
#define GREEN   "\033[92m"
#define YELLOW  "\033[93m"
#define BLUE    "\033[94m"
#define PURPLE  "\033[95m"
#define AQUA    "\033[96m"
#define ORANGE  "\033[33m"
#define ENDC    "\033[0m"

// output messages
#define msg(x)  std::cout << "[plotter] " << x << std::endl;
#define error(x) std::cout << RED << "  [error] " << ENDC << x << std::endl;

inline Int_t id_to_entry(int id) { return id-((id/100000)*100000); }
inline Int_t id_to_file(int id)  { return id/100000-1; }

#endif
