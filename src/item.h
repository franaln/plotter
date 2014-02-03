/** @file item.h */

#ifndef ITEM_H
#define ITEM_H

#include "TROOT.h"
#include <iomanip>
#include <iostream>
#include <TString.h>
#include <cmath>

#include "common.h"

typedef enum{
  Dir,
  Tree,
  Hist1D,
  Hist2D,
  Hist3D,
  Branch,
  Graph,
  Back,
  None
} ItemType;


class Item {

 private:
  Int_t     m_file;
  Int_t     m_entry;
  TString   m_name;
  TString   m_title;
  TString   m_path;
  ItemType  m_type;
  Bool_t    m_status;
  Int_t     m_id;

 public:
  Item(Int_t entry, TString name, TString title, ItemType type);

  TString GetName() { return m_name; }
  TString GetTitle() { return m_title; }
  TString GetText() { return m_name.EqualTo("") ? "no name" : m_name; }
  TString GetLegendText() { return m_title.EqualTo("") ? m_name : m_title; }
  ItemType GetType() { return m_type; };
  Bool_t GetStatus() { return m_status; }
  Int_t GetId() { return m_id; }
  Int_t GetFile() { return m_file; };
  Int_t GetEntry(){ return m_entry; };
  TString GetIcon();

  bool IsDir() { return m_type == Dir ? true : false; }
  bool IsTree() { return m_type == Tree ? true : false; }
  bool IsBack() { return m_type == Back ? true : false; }
  bool IsPlotable() { return (m_type == Hist1D || m_type==Hist2D || m_type == Hist3D || m_type == Graph || m_type == Branch) ? true : false; }
  bool IsBranch() { return m_type == Branch ? true : false; }
  bool IsTypeHist() { return (m_type == Hist1D || m_type == Hist2D || m_type == Hist3D || m_type == Branch) ? true : false; }
  bool IsTypeGraph() { return m_type == Graph ? true : false; }

  void ToggleStatus() { m_status = m_status ? false : true; }
  void SetStatus(bool st) { m_status = st; }
};


class ParentItem : public Item {

 private:
  std::vector<Item*> m_items;

 public:
  ParentItem(Int_t entry, TString name, TString title, ItemType type) : Item(entry, name, title, type) { m_items.clear(); };
  ~ParentItem();

  bool IsOpen() { return GetStatus(); }
  unsigned int GetN() { return m_items.size(); }
  void AddItem(Item* it) { m_items.push_back(it); }
  Item* GetItem(int index) { return m_items[index]; }

  Item* GetItemFromId(int id) {
    for(unsigned int k=0; k<m_items.size(); k++){
      if(m_items[k]->GetId() == id) return m_items[k];
      else continue;
    }
  }

};


#endif
