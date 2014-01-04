/* plotter : macro.h */

#ifndef MACRO_H
#define MACRO_H

#include "TROOT.h"
#include <iomanip>
#include <iostream>
#include <TString.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

enum OutputFormat {
  MRoot,
  MPython
};

struct Axis {
  TString title;
  Double_t min;
  Double_t max;
};

class HistoInfo
{
 private:
  Int_t    file;
  TString  name;
  TString  title;
  TString  drawoption;
  Int_t    rebin;
  Color_t  colour;
  Double_t scale_factor;
  TString  leg_text;

 public:
  HistoInfo(Int_t file, TString name, TString title);
  virtual ~HistoInfo();

  void SetDrawOptions(TString input){ drawoption = input; };
  void SetRebinNumber(Int_t input){ rebin = input; };
  void SetScaleFactor(Double_t input){ scale_factor = input; };
  void SetColour(Color_t input){ colour = input; };
  void SetLegendText(TString input){ leg_text = input; };
  Bool_t operator= (HistoInfo* other);

  Int_t    File() { return file; };
  TString  Name() { return name; };
  TString  DrawOption() { return drawoption; };
  Color_t  Colour() { return colour; };
  Int_t    RebinNumber() { return (rebin >1) ? rebin : 0; };
  Double_t ScaleFactor() { return scale_factor; };
  TString  MacroName();
  TString  LegendText(){ return leg_text; }; 

};

class CanvasInfo
{
  
 private:
  
  TString name;
  Axis xaxis, yaxis;
  vector<HistoInfo*> histos;

 public:
  
  CanvasInfo(TString _name);
  virtual ~CanvasInfo();

  TString         Name(){ return name; };
  //vector<int>     FilesVector(){ return files; };
  vector<HistoInfo*> Histos(){ return histos; };
  HistoInfo*         Histo(int index){ return histos[index]; };
  Int_t              NumberOfHistos(){ return histos.size(); };

  void AddHisto(HistoInfo* histo);
  void AddLegend(vector<TString>);
  
};


class Macro
{

 private:
  TString name;
  vector<TString>      files;
  vector<CanvasInfo*>  canvases;
  Int_t canvas_counter;

 public:
  Macro(TString _name);
  virtual ~Macro();
  
  void   AddFile(TString _file);
  void   AddCanvas(TString);
  void   AddHisto(HistoInfo *h);
  void   AddLegend(vector<TString>);
  void   Reset();
  void   SaveMacro(TString, OutputFormat);

};

#endif
