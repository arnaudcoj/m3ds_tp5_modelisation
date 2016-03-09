#include "WingedMap.h"
#include "Tools.h"


/**
@file
@author F. Aubert

*/


using namespace std;
using namespace p3d;


WingedMap::WingedMap() {
  //ctor
}

WingedMap::~WingedMap() {
  //dtor
}

WVertex *WingedMap::wvertex(unsigned int i) {
  return _wvertex[i];
}

WEdge *WingedMap::findEdge(WVertex *v1,WVertex *v2) {
  bool found=false;
  unsigned int i,j;
  i=0;
  while ((i<_vertexEdge[v1->index()].size()) && !found) {
    j=0;
    while ((j<_vertexEdge[v2->index()].size()) && !found) {
      if (_vertexEdge[v1->index()][i]==_vertexEdge[v2->index()][j])
        found=true;
      else
        j++;
    }

    if (!found) i++;
  }

  if (found) return _vertexEdge[v1->index()][i]; else return NULL;
}


void WingedMap::read(const string &nom,Winged *winged) {
  _wvertex.clear();
  _vertexEdge.clear();
  _mesh=new Mesh();
  _mesh->read(nom);
  _mesh->computeNormal();
  _mesh->scaleInBoxMin(-2,2,-2,2,-2,2);
  WEdge *we;
  WFace *wf;
  WVertex *wv_begin,*wv_end;

  _vertexEdge.resize(_mesh->nbPosition());
  for(unsigned int i=0;i<_mesh->nbPosition();i++) {
    _vertexEdge[i].clear();
    _wvertex.push_back(winged->createVertex(_mesh->positionMesh(i)));
  }

  for(unsigned int i=0;i<_mesh->nbFace();i++) {
    wf=winged->createFace();
    wf->normal(_mesh->normalFace(i));
    for(unsigned int j=0;j<_mesh->nbVertexFace(i);j++) {
      // On cherche et on crée éventuellement un nouveau sommet
      unsigned int v=_mesh->indexPositionVertexFace(i,j);

      wv_begin=_wvertex[v];


      // On cherche et on crée éventuellement une nouvelle arête
      unsigned int next=_mesh->indexPositionVertexFace(i,j+1);
      wv_end=_wvertex[next];

      if (wv_begin!=wv_end) {


      we=this->findEdge(wv_begin,wv_end);
      if (we) {
        if (we->begin()==wv_begin) {
          if ((we->begin()!=wv_begin) || (we->end()!=wv_end)) throw ErrorD("Edge found incoherent");
          if (we->left()!=NULL) throw ErrorD("left already set");
          we->left(wf);
        }
        else {
          if ((we->end()!=wv_begin) || (we->begin()!=wv_end)) throw ErrorD("Edge found incoherent");
          if (we->right()!=NULL) throw ErrorD("right already set");
          we->right(wf);
        }
      }
      else {
        we=winged->createEdge(wv_begin,wv_end);
        _vertexEdge[wv_begin->index()].push_back(we);
        _vertexEdge[wv_end->index()].push_back(we);
        we->left(wf);
      }

      wv_begin->edge(we);
      wv_end->edge(we);
      }
      else
        throw ErrorD("bug"); // well, hope never happens cause I dont remember why
    }
    wf->edge(we);

    unsigned int pred,succ,current0,current1;
    current0=_mesh->indexPositionVertexFace(i,0);
    current1=_mesh->indexPositionVertexFace(i,1);
    pred=_mesh->indexPositionVertexFace(i,-1);
    succ=_mesh->indexPositionVertexFace(i,2);;
    WEdge *currente,*prede,*succe;
    currente=this->findEdge(_wvertex[current0],_wvertex[current1]);
    prede=this->findEdge(_wvertex[pred],_wvertex[current0]);
    succe=this->findEdge(_wvertex[current1],_wvertex[succ]);

    for(unsigned int j=0;j<_mesh->nbVertexFace(i);j++) {
      if ((currente==succe) || (currente==prede))
        throw ErrorD("ouhla "); // means restart all the class from scratch
      if (currente->left()==wf) {
          currente->succLeft(succe);
          currente->predLeft(prede);
      }
      else {
        if (currente->right()!=wf) throw ErrorD("No face for the edge !"); // sure ?
        currente->succRight(succe);
        currente->predRight(prede);
      }
      pred=current0;
      current0=current1;
      current1=succ;
      succ=_mesh->indexPositionVertexFace(i,j+3);
      prede=currente;
      currente=succe;
      succe=this->findEdge(_wvertex[current1],_wvertex[succ]);
      if (!succe) throw ErrorD("No Succ"); // no luck
    }


  }
  delete _mesh;
}

