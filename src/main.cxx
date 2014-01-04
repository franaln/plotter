/* 
   Plotter
   -------

   Plotter is a gui interface to easily plot ROOT histograms, graphs and trees..
   Francisco Alonso <franaln@gmail.com> 
*/

#include "plotter.h"

#define VERSION "0.3.2-dev"

void ShowUsage()
{
  cout << endl;
  cout << "Plotter " << VERSION << endl;
  cout << endl;
  cout << "Usage: plotter [options] file1.root file2.root file3.root ..." << endl;
  cout << "Options:" << endl;
  cout << "   --merge (-m)            merge input files (create a chain)" << endl;
  cout << endl;
}

int main(int argc, char **argv)
{
  
  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }
  
  // Si no hay argumentos --> show version/usage
  if(argc < 2 || strcmp("-h",argv[1])==0 || strcmp("--help",argv[1])==0 ){
    ShowUsage();
    return 1;
  }

  int argpos = 1; // para saber en que posicion de argv[] estoy leyendo

  // Check option merge
  bool merge = false;
  if ( strcmp(argv[argpos], "--merge")==0 || strcmp(argv[argpos], "-m")==0) {
    merge = true;
    argpos++;
    if(argc <= argpos) { ShowUsage(); return 1; }
  }

  // Get files from args
  vector<TString> files;    
  for (int i = argpos; i < argc; i++){
    TString tmp = argv[i];
    files.push_back(tmp);
  }
  
  // Application
  TApplication *rootApp = new TApplication("Plotter", &argc, argv);

  cout << "--------------" << endl;
  cout << "|  Plotter   |" << endl;
  cout << "--------------" << endl;
  
  Plotter p(files, merge);
  
  rootApp->Run();
  
  return 0;
}
