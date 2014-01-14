/** @file item.cxx */

#include "item.h"

Item::Item(Int_t file, Int_t entry, TString name, TString title, ItemType type) :
  m_file(file),
  m_entry(entry),
  m_name(name),
  m_title(title),
  m_type(type),
  m_status(false)
{

  // Calcula id
  // file = id/100000-1
  // entry = id-((id/100000)*100000)
  m_id = 100000 * (m_file + 1) + m_entry;

}

TString Item::GetIcon()
{
  TString iconpic;
  if(m_type == Hist1D){
    iconpic = "h1_t.xpm";
  }
  else if(m_type == Hist2D){
    iconpic = "h2_t.xpm";
  }
  else if(m_type == Hist3D){
    iconpic = "h3_t.xpm";
  }
  else if(m_type == Graph){
    iconpic = "graph.xpm";
  }
  else if(m_type == Branch){
    iconpic = "leaf_t.xpm";
  }
  else if(m_type == Dir){
    iconpic = "folder_t.xpm";
  }
  else if(m_type == Tree){
    iconpic = "tree_t.xpm";
  }
  else if(m_type == Back){
    iconpic = "folder_t.xpm";
  }

  return iconpic;
}
