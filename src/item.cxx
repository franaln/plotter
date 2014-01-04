/* plotter : item.cxx */

#include "item.h"

Item::Item(Int_t _nfile, Int_t _nentry, TString _name, TString _title, ItemType _type) :
  nfile(_nfile),
  nentry(_nentry),
  name(_name),
  title(_title),
  type(_type),
  status(false)
{

  // Calcula id
  // file = id/100000-1 
  // entry = id-((id/100000)*100000)
  id = 100000 * (nfile + 1) + nentry;

}

Item::~Item() 
{

}

const TGPicture* Item::GetIcon()
{
  const TGPicture *iconpic;

  if(type == H1){
    iconpic = gClient->GetPicture("h1_t.xpm");
  }
  else if(type == H2){
    iconpic = gClient->GetPicture("h2_t.xpm");
  }
  else if(type == H3){
    iconpic = gClient->GetPicture("h3_t.xpm");
  }
  else if(type == Graph){
    iconpic = gClient->GetPicture("graph.xpm");
  }
  else if(type == Branch){
    iconpic = gClient->GetPicture("leaf_t.xpm");
  }
  else if(type == Dir){
    iconpic = gClient->GetPicture("folder_t.xpm");
  }
  else if(type == Tree){
    iconpic = gClient->GetPicture("tree_t.xpm");
  }
  else if(type == Back){
    iconpic = gClient->GetPicture("folder_t.xpm");
  }

  return iconpic;
}


