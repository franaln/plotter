/** @file base.h
    @brief Common definitions and fns */

#ifndef COMMON_H
#define COMMON_H

#include <iostream>

// output messages
#define msg(x)  std::cout << "-- " << x << std::endl;
#define error(x) std::cout << "\033[91merror!\033[0m " << x << std::endl;

// id <-> (file, entry)
inline Int_t id_to_entry(int id) { return id-((id/100000)*100000); }
inline Int_t id_to_file(int id)  { return id/100000-1; }

#endif
