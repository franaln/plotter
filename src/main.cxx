/**
   @file main.cxx
   @brief Plotter is a gui interface to easily plot ROOT histograms, graphs and trees..
   @author Francisco Alonso <franaln@gmail.com>
   @version 0.4

   @mainpage plotter
   @brief GUI interface to easily plot ROOT histograms, trees or graphs
*/

#define NAME    "plotter"
#define VERSION "0.4"

#include "plotter.h"

void show_usage()
{
  std::cout << NAME << " " << VERSION << std::endl;
  std::cout << std::endl;
  std::cout << "Usage: " << NAME << " [options] file1.root file2.root file3.root ..." << std::endl;
  std::cout << std::endl;
}

int main(int argc, char **argv)
{

  if(gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

  // If no arguments: show version/usage
  if(argc < 2 || strcmp("-h",argv[1])==0 || strcmp("--help",argv[1])==0 ){
    show_usage();
    return 1;
  }

  int argpos = 1; // para saber en que posicion de argv[] estoy leyendo

  // Check option merge
  bool merge = false;
  if ( strcmp(argv[argpos], "--merge")==0 || strcmp(argv[argpos], "-m")==0) {
    merge = true;
    argpos++;
    if(argc <= argpos) {
      show_usage();
      return 1;
    }
  }

  // Get files from args
  std::vector<TString> files;
  for (int i = argpos; i < argc; i++){
    TString tmp = argv[i];
    files.push_back(tmp);
  }

  // Application
  TApplication *rootApp = new TApplication("Plotter", &argc, argv);

  std::cout << NAME << " " << VERSION << std::endl;

  Plotter p(files, merge);

  rootApp->Run();

  return 0;
}
