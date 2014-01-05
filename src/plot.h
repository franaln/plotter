/** @file plot.h
    @brief Header file for plot class
*/

#ifndef PLOT_H
#define PLOT_H

#include <TROOT.h>
#include <TLegend.h>
#include <TCanvas.h>


/** Base class for a root plot
 */
class Plot {

 public:
  Plot();
  ~Plot();

  void Add(TObject*);
  void Show();
  void Save();

  static int number_of_plot;

 private:
  TString m_name;
  TCanvas *m_canvas;
  TLegend *m_legend;
  TList   *m_list;

  void Create();
  void CreateLegend();
};

#endif
