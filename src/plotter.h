/** @file plotter.h
*/

#ifndef PLOTTER_H
#define PLOTTER_H

//C++
#include <iostream>

//ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TStyle.h>
#include <TKey.h>
#include <TList.h>
#include <TCanvas.h>
#include <TString.h>
#include <TColor.h>
#include <TMath.h>
#include <TGraphAsymmErrors.h>

//GUI
#include <TApplication.h>
#include <TGFrame.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFontDialog.h>
#include <TGFSContainer.h>
#include <TGString.h>
#include <TGButton.h>
#include <TGColorSelect.h>
#include <TGClient.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGLayout.h>
#include <TGMenu.h>
#include <TGFileDialog.h>
#include <TGStatusBar.h>
#include <TGListTree.h>

//plotter
#include "common.h"
#include "macro.h"

class Item;
class ItemsBox;
class Obj;
class Plot;

class Plotter : public TGMainFrame {

 public:
  Plotter(std::vector<TString> files, bool merge=false);
  virtual ~Plotter();

  // Slots (must be public!)
  void OnItemClick(Int_t);
  void OnItemDoubleClick(TGFrame*, Int_t);
  void OnButtonClearSelection() { ClearSelection(); }
  void OnButtonDraw() { Draw(); }
  void OnButtonDrawEfficiency() { DrawEfficiency(); }
  void OnButtonDrawRatio() { DrawRatio(); }
  void OnButtonExit() { Exit(); }
  void ShowHideColours();
  void ShowHideCuts();

 private:
  // Gui widgets
  TGCompositeFrame *frame_main;
  TGCompositeFrame *frame_aux;
  TGCompositeFrame *frame_checks;
  TGCompositeFrame *frame_hist_options;
  TGCompositeFrame *frame_hist2D_options;
  TGCompositeFrame *frame_hist3;
  TGCompositeFrame *frame_rebin;
  TGCompositeFrame *frame_log;
  TGVerticalFrame *frame_column_frame[15];
  TGVerticalFrame *frame_options;
  TGVerticalFrame *frame_colours;
  TGHorizontalFrame *frame_row[3];
  TGStatusBar *status_bar;
  TGLayoutHints *layout_buttons;
  TGLayoutHints *layout_menu_bar;
  TGLayoutHints *layout_menu_bar_item;
  TGLayoutHints *layout_checks;
  TGTextButton  *button_clear_selection;
  TGTextButton *button_draw;
  TGTextButton *button_draw_efficiency;
  TGTextButton *button_draw_ratio;
  TGTextButton *button_draw_and_ratio;
  TGTextButton *button_draw_and_diff;
  TGTextButton *button_exit;
  TGTextButton *button_save_colours;
  TGGroupFrame *group_options;
  TGGroupFrame *group_hist_options;
  TGGroupFrame *group_hist2D_options;
  TGGroupFrame *group_colours;
  TGTextEntry  *entry_file[15];
  TGTextEntry  *entry_cuts;
  TGLabel *label_rebin;
  TGLabel *label_hist2;
  TGLabel *label_status;
  TGNumberEntry *nentry_rebin;
  TGCheckButton *check_normalise;
  TGCheckButton *check_normalise2;
  TGCheckButton *check_hist;
  TGCheckButton *check_scatter;
  TGCheckButton *check_text;
  TGCheckButton *check_p;
  TGCheckButton *check_box;
  TGCheckButton *check_stats;
  TGCheckButton *check_atlas;
  TGCheckButton *check_log_x;
  TGCheckButton *check_log_y;
  TGCheckButton *check_order;
  TGCheckButton *check_pie;
  TGCheckButton *check_include_diff;
  TGCheckButton *check_include_ratio;
  TGRadioButton *radio_colz;
  TGRadioButton *radio_scatter;
  TGRadioButton *radio_box;
  TGMenuBar *menu_bar;
  TGPopupMenu *menu_file;
  TGPopupMenu *menu_macro;
  TGPopupMenu *menu_boxes;
  TGPopupMenu *menu_help;
  TGPopupMenu *menu_view;
  TGColorSelect *colorselect[20];
  TGCheckButton *check_fill[20];
  ItemsBox *boxes[15];

  void CreateMainWindow();
  void CreateMainFrame();
  void CreateOptionsFrame();
  void CreateMenuBar();
  void CreateStatusBar();
  void CreateColoursFrame();
  void CreateCutsEntry();
  void CreateMergedFileBox();
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);

  inline void Exit() {
    msg("Bye :)");
    CloseWindow();
  }

  inline void ExitError() {
    msg("Bye :(");
    CloseWindow();
  }

  void ClearSelection();
  void SavePlots();
  inline void CloseWindow() { gApplication->Terminate(0); }
  void LoadSettings();
  //void SaveSettings();
  void GetColours();
  Color_t ConvertStringToColour(const char *c);
  void SetStyle();
  void ConfigurePlotList();
  void Draw();
  void DrawEfficiency();
  void DrawRatio();
  std::vector<int> GetNumberOfObjectsInEachFile();
  void CreateMacro(OutputFormat);

  Obj* GetObject(Item* it);

  UInt_t m_number_of_files;
  std::vector<TString> m_file_names;
  std::vector<Item*> m_items;
  std::vector<Plot*> m_plots;
  Double_t x_min, x_max, y_min, y_max;
  Pixel_t pcolors[20];
  Color_t colours[20];
  TStyle *plotter_style;
  short marker_style;
  float marker_size;
  short line_width;
  //  Macro *macro;
  TChain *merge_chain;

  Bool_t m_merge_mode;
  Bool_t m_macro_recording;

  ClassDef(Plotter, 0);
};
#endif //PLOTTER_H
