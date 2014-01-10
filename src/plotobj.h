/** @file plotobj.h
    @brief Header file for plotobj class
*/

#ifndef PLOTOBJ_H
#define PLOTOBJ_H

#include <TROOT.h>
#include <TLegend.h>
#include <TCanvas.h>

#include <TH1.h>
#include <TGraph.h>


/** Wrapper of all root plotable objects: TH1, TGraph, Branch...
 */
class PlotObj {

 private:
  enum {Hist, Graph} m_type;

  TH1 *m_hist;
  TGraph *m_graph;

 public:
 PlotObj(TH1 *obj) : m_type(Hist), m_hist(obj) {}
 PlotObj(TGraph *obj) : m_type(Graph), m_graph(obj)  {}
  ~PlotObj() { delete m_hist; delete m_graph; }

  Double_t GetMinX();
  Double_t GetMaxX();
  Double_t GetMinY();
  Double_t GetMaxY();

  void SetColour(Color_t);

  void Draw();
};

#endif
