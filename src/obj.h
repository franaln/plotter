/** @file obj.h
    @brief Header file for obj class
*/

#ifndef OBJ_H
#define OBJ_H

#include <TROOT.h>
#include <TLegend.h>
#include <TCanvas.h>

#include <TH1.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>

/** Wrapper for root plotable objects: Histograms and Graphs
 */
class Obj {

 private:
  enum {Hist, Graph} m_type;

  TH1    *m_hist;
  TGraph *m_graph;

  TString m_opts;

 public:
  Obj(TH1 *obj) : m_type(Hist), m_hist(obj), m_opts("") { }
  Obj(TGraph *obj) : m_type(Graph), m_graph(obj), m_opts("") { }
  Obj(TH1*, TH1*, std::string);

  ~Obj() { delete m_hist; delete m_graph; }

  void Rebin(int);
  void NormaliseTo(double);
  double Integral();

  // getters
  TString GetName();
  double GetMinX();
  double GetMaxX();
  double GetMinY();
  double GetMaxY();

  // setters
  void SetTitleX(TString);
  void SetTitleY(TString);
  void SetRangeX(double, double);
  void SetRangeY(double, double);
  void SetStats(bool stat) { if(m_type == Hist) m_hist->SetStats(stat); }
  void SetColour(Color_t, bool);
  void SetStyle();

  void Draw(TString options="");
};

#endif
