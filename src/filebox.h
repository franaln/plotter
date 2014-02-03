/** @file filebox.h */

#ifndef FILEBOX_H
#define FILEBOX_H

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
  FileBox(TGWindow *main, UInt_t, UInt_t, TString);
  ~FileBox();

  Item* GetItem(int entry) { return parent->GetItem(entry); };
  TString GetHeaderText() { return m_header->GetText(); };
  TGListBox* GetContent() { return m_content; };
  TFile* GetFile() { return m_file; };
  TDirectory* GetCurrentDir()  { return m_file->GetDirectory(current_path); };
  TTree* GetCurrentTree() {  return (TTree*)m_file->Get(current_path); };

  void Clear();

  //slots
  void OnItemDoubleClick(TGFrame*, Int_t);
  void OnItemClick(Int_t);

 protected:
  void CreateGui(TString);
  void RefreshGui();

  void BrowseDir(ParentItem*);
  void BrowseTree(ParentItem*);

  void ShowItems();
  void OpenItem(int);
  void CloseItem(int);

  TFile *m_file;
  //  std::vector<Item*> m_items;

  ParentItem *parent;
  int entry;

  TString last_path, current_path;

  //gui
  TGTextEntry *m_header;
  TGListBox   *m_content;

  ClassDef(FileBox, 0);
};
#endif
