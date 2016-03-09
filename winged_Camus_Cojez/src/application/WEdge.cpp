
#include "WEdge.h"

#include <vector>

#include "glsupport.h"
#include "Tools.h"

#include "WFace.h"
#include "GLTool.h"


/**
@file
@author F. Aubert

@brief Winged-Edges : opérations sur les arêtes
*/

using namespace p3d;
using namespace std;



/** ************************************************************************************
** A COMPLETER : WEdge::computePointEdge => calcul des sommets d'arête
** c'est la moyenne entre les milieux d'aretes et des points de faces adjacents.
** les points de face adjacents sont accessibles par this->left()->pointFace(), et this->right()->pointFace() (de type Vector3).
** le calcul du milieu se fait avec e->begin()->point() et e->end()->point() (de type Vector3 qui permet les opérations +,/, etc).
** ************************************************************************************ */
void WEdge::computePointEdge() {
  _pointEdge.set(0,0,0);
  unsigned int nb=0;
  if (left()) {_pointEdge+=left()->pointFace();nb++;}
  if (right()) {_pointEdge+=right()->pointFace();nb++;}
  _pointEdge+=begin()->position()+end()->position();
  _pointEdge=_pointEdge/(2.0+nb);

}


/** ************************************************************************************
** A COMPLETER : WEdge::insertVertexPointEdge => insertion d'un sommet correspondant au sommet d'arête.
** ************************************************************************************ */

void WEdge::insertVertexPointEdge() {
  WEdge *new_edge;
  WVertex *new_vertex;

  // construction du nouveau sommet
  new_vertex=_owner->createVertex(pointEdge());
  new_vertex->edge(this); // seul champ à mettre à jour

  // construction de la nouvelle arête (new_edge->begin(), et new_edge->end() déjà affecté par la construction).
  new_edge=_owner->createEdge(new_vertex,end());


  // il faut mettre à jour tous les champs de new_edge, mais également de tous les champs des entités adjacentes
  // (par exemple il faut changer le successeur, ou le prédécesseur selon le sens, de this->succRight()...).
  if(this->succLeft()->begin()==new_edge->end())
    this->succLeft()->predLeft(new_edge);
  else
    this->succLeft()->predRight(new_edge);
  if(this->predRight()->end()==new_edge->end())
    this->predRight()->succLeft(new_edge);
  else
    this->predRight()->succRight(new_edge);
//  new_edge->succLine(this->succLine());
  new_edge->left(this->left());
  new_edge->right(this->right());
  new_edge->succLeft(this->succLeft());
  new_edge->predLeft(this);
  new_edge->succRight(this);
  new_edge->predRight(this->predRight());
  this->succLine(new_edge);
  this->succLeft(new_edge);
  this->predRight(new_edge);
  this->end(new_edge->begin());

  new_edge->end()->edge(new_edge);


}


/** ************************************************************************************ */



// start should be a boundary edge
void WEdge::followBoundary() {
  WEdge *e;
  WEdge *next;
  e=this;
  unsigned int number=0;
  do {
      if (e->left()==NULL) next=e->end()->directBoundary();
      else if (e->right()==NULL) next=e->begin()->directBoundary();
      else throw ErrorD("WEdge::followBoundary : not a boundary edge");
      e->succLine(next);
      e->mark();
      e=next;
      number++;
      // if (e->isMark() && e!=start) {cout << "bouclage" << e <<endl;error("problème de link",__LINE__,__FILE__);}
  } while (e!=this);
}


WEdge::~WEdge() {
  //dtor
}

WEdge::WEdge(WVertex *b,WVertex *e,Winged *owner) {
  _begin=b;
  _end=e;
  _owner=owner;
  computeVector();
  _succL=NULL;_succR=NULL;
  _predL=NULL;_predR=NULL;
  _left=NULL;_right=NULL;
}

Winged *WEdge::owner() {
  return _owner;
}

WVertex *WEdge::begin() const {
  return _begin;
}

WVertex *WEdge::end() const {
  return _end;
}

void WEdge::begin(WVertex *b) {
  _begin=b;
  if (_end) computeVector();
}

void WEdge::end(WVertex *e) {
  _end=e;
  if (_begin) computeVector();
}

WFace *WEdge::left() const {
  return _left;
}

WFace *WEdge::right() const {
  return _right;
}

void WEdge::left(WFace *f) {
  _left=f;
}

void WEdge::right(WFace *f) {
  _right=f;
}

WEdge *WEdge::succLeft() const {
  return _succL;
}

WEdge *WEdge::succRight() const {
  return _succR;
}

WEdge *WEdge::predLeft() const {
  return _predL;
}

WEdge *WEdge::predRight() const {
  return _predR;
}

void WEdge::succLeft(WEdge *e) {
  _succL=e;
}

void WEdge::succRight(WEdge *e) {
  _succR=e;
}
void WEdge::predLeft(WEdge *e) {
  _predL=e;
}
void WEdge::predRight(WEdge *e) {
  _predR=e;
}

Vector3 WEdge::midPoint() {
  Vector3 res;
  res=(begin()->position()+end()->position())/2.0;
  return res;
}

void WEdge::draw() {
}

void WEdge::computeVector() {
  _vec.setVector(begin()->position(),end()->position());
//  _vector.normalize();
}

const Vector3 &WEdge::vec() const {
  return _vec;
}

Vector3 WEdge::direction() {
  Vector3 res;
  if (_vec.length2()>0.00000001) {
    res=_vec;
    res.normalize();
    return res;
  }
  else return Vector3(0,0,0);
}

unsigned int WEdge::index() {
  return _index;
}

void WEdge::index(unsigned int i) {
  _index=i;
}


const Vector3 &WEdge::pointEdge() const {
  return _pointEdge;
}


void WEdge::mark() {
  _mark=true;
}

void WEdge::unmark() {
  _mark=false;
}

bool WEdge::isMark() {
  return _mark;
}

void WEdge::drawLine() {
  vector<int> a;
  vector<Vector3> pts;
  pts.clear();
  unsigned int num=0;
  WEdge *e,*start;
  WVertex *v;
  start=this;
  e=start;
  if (start->succLine()) {
    if (begin()==succLine()->begin()) v=end(); else v=begin();
  }
  do {
    pts.push_back(e->begin()->position());
    pts.push_back(e->end()->position());
    e=e->succLine();
    if (e) {if (v==e->begin()) v=e->end(); else v=e->begin();}
    num++;
  } while ((e!=start) && e);

  p3d::drawLines(pts);
}


void WEdge::succLine(WEdge *e) {
  _succLine=e;
}

WEdge *WEdge::succLine() {
  return _succLine;
}

ostream& operator <<(std::ostream &s,const WEdge &e) {
    s << "(begin = " << e.begin() << ", end= " << e.end() << "left,right = " <<  e.left() << "," << e.right() << " succLeft = " << e.succLeft() << " succRight= " << e.succRight() << " predLeft = " << e.predLeft() << " predRight=" << e.predRight();
    return s;
}




