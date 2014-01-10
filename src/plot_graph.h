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

  void Add(TObject*, Color_t colour=kBlack/*, ...*/);
  void Show();
  void Save();
  void Dump();

  void SetIncludeRatio(bool set) { include_ratio = set; }
  void SetIncludeDiff(bool set) { include_diff = set; }

  static int number_of_plot;

 private:
  void Create();
  void CreateLegend();

  TString m_name;
  TCanvas *m_canvas;
  TLegend *m_legend;

  TList *m_list;
  std::vector<short> m_colours;

  double x_min, x_max, y_min, y_max;

  bool include_ratio;
  bool include_diff;

  bool do_logx;
  bool do_logy;
  bool do_normalise;
  bool do_normalise_to_first;
  bool show_stats;
};

#endif
