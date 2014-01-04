#ifndef BASE_H
#define BASE_H

//
#define MSG(x) cout << ":: " << x << endl;
#define ERROR(x,y) cout << "Error in <Plotter::" << x << ">: " << y << endl;

// id <-> (file, entry)
inline Int_t IdToEntry(int id) { return id-((id/100000)*100000); }
inline Int_t IdToFile(int id)  { return id/100000-1; }

#endif
