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

  void SetDrawOptions(TString input){ m_drawoption = input; };
  void SetRebinNumber(Int_t input){ m_rebin = input; };
  void SetScaleFactor(Double_t input){ m_scale_factor = input; };
  void SetColour(Color_t input){ m_colour = input; };
  void SetLegendText(TString input){ m_leg_text = input; };
  Bool_t operator= (HistoInfo* other);

  Int_t    GetFile() { return m_file; };
  TString  GetName() { return m_name; };
  TString  GetDrawOption() { return m_drawoption; };
  Color_t  GetColour() { return m_colour; };
  Int_t    GetRebinNumber() { return (m_rebin >1) ? m_rebin : 0; };
  Double_t GetScaleFactor() { return m_scale_factor; };
  TString  GetMacroName();
  TString  GetLegendText(){ return m_leg_text; };

};

class CanvasInfo {

 private:
  TString m_name;
  Axis m_xaxis, m_yaxis;
  std::vector<HistoInfo*> m_histos;

 public:
 CanvasInfo(TString name) : m_name(name) {};

  TString GetName() { return m_name; };
  std::vector<HistoInfo*> GetHistos(){ return m_histos; };
  HistoInfo* GetHisto(int index){ return m_histos[index]; };
  Int_t GetNumberOfHistos(){ return m_histos.size(); };

  void AddHisto(HistoInfo* histo);
  void AddLegend(std::vector<TString>);

};


class Macro
{

 private:
  TString m_name;
  std::vector<TString> m_files;
  std::vector<CanvasInfo*>  m_canvases;
  Int_t canvas_counter;

 public:
  Macro(TString name) : m_name(name) {};
  virtual ~Macro();

  void   AddFile(TString);
  void   AddCanvas(TString);
  void   AddHisto(HistoInfo *h);
  void   AddLegend(std::vector<TString>);
  void   Reset();
  void   SaveMacro(TString, OutputFormat);

};

#endif
