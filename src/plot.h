/** @file plot.h
    @brief Header file for plot class
*/

#ifndef PLOT_H
#define PLOT_H

#include <TROOT.h>
#include <TLegend.h>
#include <TCanvas.h>


class Obj;

/** Base class for a root plot
 */
class Plot {

 public:
  Plot();
  ~Plot();

  void Add(Obj*, Color_t colour=kBlack, bool=false);
  void Create();
  void Save();
  void Dump();

  void SetIncludeRatio(bool set) { include_ratio = set; }
  void SetIncludeDiff(bool set) { include_diff = set; }
  void SetDrawOptions(TString opts) { draw_options = opts; }
  static int number_of_plot;

 private:
  void Configure();
  void Draw();
  void DrawEfficiency();
  void DrawRatios();
  //void DrawDiffs();
  void CreateLegend();

  TString m_name;
  TCanvas *m_canvas;
  TLegend *m_legend;
  std::vector<Obj*> m_list;

  double x_min, x_max, y_min, y_max;
  bool include_ratio;
  bool include_diff;
  TString draw_options;
  int rebin;
  bool do_logx;
  bool do_logy;
  bool do_normalise;
  bool do_normalise_to_first;
  bool show_stats;
};

#endif
