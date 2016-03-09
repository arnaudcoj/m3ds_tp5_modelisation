#include "Winged.h"
#include <iostream>
#include "WingedMap.h"
#include "GLTool.h"
#include "Tools.h"

/**
@file
@author F. Aubert
@brief  Winged-Edges : stockage et construteurs des entités d'un winged + opérations sur l'objet

*/

using namespace std;
using namespace p3d;



/** ************************************************************************************
**   Winged::catmullClark()
** *********************************************************************************** */

void Winged::catmullClark() {
  computePointFace();
  computePointEdge();
  computePointVertex();

  // les coordonnées des sommets actuels deviennent celles des points de sommets.
  unsigned int nb_vertex=nbVertex();
  for(unsigned int i=0;i<nb_vertex;i++) {
    vertex(i)->position(vertex(i)->pointVertex());
  }

  // insertion des sommets d'arêtes
  unsigned int nb_edge=nbEdge();
  for(unsigned int i=0;i<nb_edge;i++) {
    edge(i)->insertVertexPointEdge();
  }

// /*
  // insertion des faces
  unsigned int nb_face=nbFace();
  for(unsigned int i=0;i<nb_face;i++) {
    face(i)->insertVertexPointFace();
  }

  for(unsigned int i=0;i<nb_face;i++) {
    eraseFace(0);
  }
// */
}




/** ************************************************************************************ */


Winged::Winged() {
  //ctor
}

Winged::~Winged() {
  //dtor
  for(unsigned int i=0;i<nbVertex();i++) {
    delete _vertex[i];
  }
  for(unsigned int i=0;i<nbEdge();i++) {
    delete _edge[i];
  }
  for(unsigned int i=0;i<nbFace();i++) {
    delete _face[i];
  }
}

void Winged::read(string nom) {
    WingedMap transfert;
    transfert.read(nom,this);
    computeNormalFace();
}

unsigned int Winged::nbVertex() {
  return _vertex.size();
}

// WARNING : no check up to delete all edges/vertices that are not attached to any face after deletion (call deleteFace to do this).
void Winged::eraseFace(unsigned int i) {
  WFace *f=face(i);
  _face.erase(_face.begin()+i);
  delete f;
}


void Winged::deleteFace(unsigned int i) {
  i=i;
  throw ErrorD("well... not done yet...");
}

WVertex *Winged::createVertex(const Vector3 &p) {
  WVertex *res;
  res=new WVertex(p,this);
  _vertex.push_back(res);
  res->index(_vertex.size()-1);
  return res;
}

WFace *Winged::createFace() {
  WFace *res;
  res=new WFace(this);
  _face.push_back(res);
  return res;
}

WEdge *Winged::createEdge(WVertex *b,WVertex *e) {
  WEdge *res;
  res=new WEdge(b,e,this);
  _edge.push_back(res);
  return res;
}


WEdge *Winged::findEdge2(WVertex *v1,WVertex *v2) {
  unsigned int i=0;
  bool found=false;
  while (i<_edge.size() && !found) {
    if (((_edge[i]->begin()==v1) && (_edge[i]->end()==v2)) || ((_edge[i]->end()==v1) && (_edge[i]->begin()==v2)))
        found=true;
    else i++;
  }
  if (found) return _edge[i];
  else return NULL;
}

unsigned int Winged::nbFace() {
  return _face.size();
}

unsigned int Winged::nbEdge() {
  return _edge.size();
}

void Winged::drawPointFace() {
  for(unsigned int i=0;i<nbFace();i++) {
    p3d::drawPoints({face(i)->pointFace()});
  }
}

void Winged::drawPointVertex() {
  for(unsigned int i=0;i<nbVertex();i++) {
    p3d::drawPoints({vertex(i)->pointVertex()});
  }
}


WFace *Winged::face(int i) {
  return _face[i];
}

WVertex *Winged::vertex(int i) {
  return _vertex[i];
}

WEdge *Winged::edge(int i) {
  return _edge[i];
}

void Winged::computeNormalFace() {
  for(unsigned int i=0;i<nbFace();i++) {
    face(i)->computeNormal();
  }
}

void Winged::drawLineCatmull() {
  for(unsigned int i=0;i<nbVertex();i++) {
    vertex(i)->drawLineCatmull();
  }
  for(unsigned int i=0;i<nbFace();i++) {
    face(i)->drawLineCatmull();
  }
}

void Winged::computeNormalVertex() {
  for(unsigned int i=0;i<nbVertex();i++) {
    vertex(i)->computeNormal();
  }
}

void Winged::computePointFace() {
  for(unsigned int i=0;i<nbFace();i++) {
    face(i)->computePointFace();
  }
}

void Winged::computePointEdge() {
  for(unsigned int i=0;i<nbEdge();i++) {
    edge(i)->computePointEdge();
  }
}

void Winged::computePointVertex() {
  for(unsigned int i=0;i<nbVertex();i++) {
    vertex(i)->computePointVertex();
  }
}


void Winged::initLineAttrib() {
  _lineAttrib.clear();
  for(WEdge *e:_edge) {
    _lineAttrib.push_back(e->begin()->position());
    _lineAttrib.push_back(e->end()->position());
  }
}

void Winged::draw(bool withNormal) {
  for(unsigned int i=0;i<nbFace();i++) {
    face(i)->draw(withNormal);
  }
}


void Winged::markAll() {
  for(unsigned int i=0;i<nbEdge();i++) {
    edge(i)->mark();
  }
}

vector<WEdge*> Winged::boundary() {
  vector<WEdge*> res;
  res.clear();
  unmarkAll();
  for(unsigned int i=0;i<nbEdge();i++) {
    if (!edge(i)->isMark()) {
      if ((edge(i)->left()==NULL) || (edge(i)->right()==NULL)) {
        edge(i)->followBoundary();
        res.push_back(edge(i));
      }
  }
  }
  cout << "Boundaries found = " << res.size() << endl;
  return res;
}

void Winged::checkVertexBound() {
  for(unsigned int i=0;i<nbVertex();i++) {
    vertex(i)->checkVertexBound();
  }
}

void Winged::checkEdgeFree() {
  for(unsigned int i=0;i<nbEdge();i++) {
    if ((edge(i)->left()==NULL) && (edge(i)->right()==NULL))
        cout << "Arete Libre" << endl;
  }
}




void Winged::unmarkAll() {
  for(unsigned int i=0;i<nbEdge();i++) {
    edge(i)->unmark();
  }
}


void Winged::drawWire(const Vector4 &color) {
  p3d::ambientColor=color;
  p3d::shaderVertexAmbient();
  p3d::drawLines(_lineAttrib);
}

void Winged::drawEdgeBoundary() {
  for(unsigned int i=0;i<nbEdge();i++) {
    WEdge *e=edge(i);
    if ((e->left()==NULL) || (e->right()==NULL)) {
      e->draw();
    }
  }
}

void Winged::drawPointEdge() {
  for(unsigned int i=0;i<nbEdge();i++) {
    p3d::drawPoints({edge(i)->pointEdge()});
  }
}


void Winged::index() {
  for(unsigned int i=0;i<nbVertex();i++) {
    vertex(i)->index(i);
  }
  for(unsigned int i=0;i<nbFace();i++) {
    face(i)->index(i);
  }
  for(unsigned int i=0;i<nbEdge();i++) {
    edge(i)->index(i);
  }
}

Winged *Winged::clone() {
  Winged *res;
  res=new Winged();
  for(unsigned int i=0;i<nbVertex();i++) {
    res->createVertex(vertex(i)->position());
  }
  for(unsigned int i=0;i<nbFace();i++) {
    res->createFace();
  }
  for(unsigned int i=0;i<nbEdge();i++) {
    res->createEdge(res->vertex(edge(i)->begin()->index()),res->vertex(edge(i)->end()->index()));
  }

  for(unsigned int i=0;i<nbVertex();i++) {
    res->vertex(i)->edge(res->edge(vertex(i)->edge()->index()));
  }

  for(unsigned int i=0;i<nbFace();i++) {
    res->face(i)->edge(res->edge(face(i)->edge()->index()));
  }

  for(unsigned int i=0;i<nbEdge();i++) {
    res->edge(i)->left(res->face(edge(i)->left()->index()));
    res->edge(i)->right(res->face(edge(i)->right()->index()));
    res->edge(i)->succRight(res->edge(edge(i)->succRight()->index()));
    res->edge(i)->succLeft(res->edge(edge(i)->succLeft()->index()));
    res->edge(i)->predRight(res->edge(edge(i)->predRight()->index()));
    res->edge(i)->predLeft(res->edge(edge(i)->predLeft()->index()));
  }

  return res;
}

void Winged::check() {
  cout << "checking..." << endl;
  for(unsigned int i=0;i<nbFace();i++) {
    cout << "Face =" << i << endl;
    if (face(i)->edge()==NULL) ErrorD("edge null dans face");
    WEdge *start=face(i)->edge();
    WEdge *e=start;
    do {
      if (e==NULL) throw ErrorD("Succession NULL");
      cout << "Edge = " << e << endl;
      if (e->left()==face(i)) {
        e=e->succLeft();
      }
      else {
        e=e->succRight();
      }
    } while (e!=start);
  }
  cout << "checked" << endl;
}


void Winged::drawNormal() {
    vector<Vector3> lines;
    for(unsigned int i=0;i<_vertex.size();++i) {
        lines.push_back(_vertex[i]->position());
        lines.push_back(_vertex[i]->normal()*0.2+_vertex[i]->position());
    }
    p3d::ambientColor=Vector4(1,0,0,1);
    p3d::shaderVertexAmbient();
    p3d::drawLines(lines);
}

void Winged::drawEdgeBound(const Vector4 &c) {

}





void Winged::stats() {
  cout << "*******************" << endl;
  cout << "nbVertex = " << nbVertex() << endl;
  cout << "nbEdge = " << nbEdge() << endl;
  cout << "nbFace = " << nbFace() << endl;
  cout << "*******************" << endl;
}


