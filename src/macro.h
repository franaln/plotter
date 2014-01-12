/** @file macro.h
*/

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
  Int_t    m_file;
  TString  m_name;
  TString  m_title;
  TString  m_drawoption;
  Int_t    m_rebin;
  Color_t  m_colour;
  Double_t m_scale_factor;
  TString  m_leg_text;

 public:
  HistoInfo(Int_t file, TString name, TString title);
  virtual ~HistoInfo();

  void SetDrawOptions(TString input){ m_drawoption = input; };
  void SetRebinNumber(Int_t input){ m_rebin = input; };
  void SetScaleFactor(Double_t input){ m_scale_factor = input; };
  void SetColour(Color_t input){ m_colour = input; };
  void SetLegendText(TString input){ m_leg_text = input; };
  Bool_t operator= (HistoInfo* other);

  Int_t    GetFile() { return m_file; };
  TString  GetName() { return m_name; };
  TString  GetDrawOption() { return drawoption; };
  Color_t  GetColour() { return colour; };
  Int_t    GetRebinNumber() { return (rebin >1) ? rebin : 0; };
  Double_t GetScaleFactor() { return scale_factor; };
  TString  GetMacroName();
  TString  GetLegendText(){ return leg_text; };

};

class CanvasInfo
{

 private:

  TString name;
  Axis xaxis, yaxis;
  std::vector<HistoInfo*> histos;

 public:

  CanvasInfo(TString _name);
  virtual ~CanvasInfo();

  TString         Name(){ return name; };
  std::vector<HistoInfo*> Histos(){ return histos; };
  HistoInfo*         Histo(int index){ return histos[index]; };
  Int_t              NumberOfHistos(){ return histos.size(); };

  void AddHisto(HistoInfo* histo);
  void AddLegend(std::vector<TString>);

};


class Macro
{

 private:
  TString name;
  std::vector<TString>      files;
  std::vector<CanvasInfo*>  canvases;
  Int_t canvas_counter;

 public:
  Macro(TString _name);
  virtual ~Macro();

  void   AddFile(TString _file);
  void   AddCanvas(TString);
  void   AddHisto(HistoInfo *h);
  void   AddLegend(std::vector<TString>);
  void   Reset();
  void   SaveMacro(TString, OutputFormat);

};

#endif
