/** @file filebox.h
*/

#ifndef ITEMSBOX_H
#define ITEMSBOX_H

#include <iostream>

#include <TROOT.h>
#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGTextEntry.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TChain.h>

#include "common.h"
#include "item.h"

class FileBox  : public TGVerticalFrame {

public:
  FileBox(TGWindow *main, UInt_t, UInt_t, Int_t, TString);
  ~FileBox();

  Item* GetItem(int entry) { return m_items[entry]; };
  TString GetHeaderText() { return m_header->GetText(); };
  TGListBox* GetContent() { return m_content; };
  TFile* GetFile() { return m_file; };
  TDirectory* GetCurrentDir()  { return m_file->GetDirectory(current_path); };
  TTree* GetCurrentTree() {  return (TTree*)m_file->Get(current_path); };

  void Clear();

  //slot
  void OnItemDoubleClick(TGFrame*, Int_t);
  void OnItemClick(Int_t);

 protected:
  void CreateGui(TString);
  void RefreshGui();

  void BrowseItems(TString fname="");
  void BrowseTree(TString name);
  void GoBack() { BrowseItems(last_path); }
  void ShowItems();
  TString GetFilenameFromPath(TString);

  Int_t m_column;;

  TFile *m_file;
  std::vector<Item*> m_items;

  TString last_path, current_path;

  //gui
  TGTextEntry *m_header;
  TGListBox   *m_content;

  ClassDef(FileBox, 0);
};
#endif
