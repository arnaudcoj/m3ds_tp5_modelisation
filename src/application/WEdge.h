/**
@file
@author F. Aubert


*/

#ifndef WEDGE_H
#define WEDGE_H

class WVertex;
class WFace;
class Winged;

#include "Vector3.h"

class WEdge
{
  public:
    WEdge(WVertex *v1,WVertex *v2,Winged *prop);
    virtual ~WEdge();

    unsigned int index();
    void index(unsigned int i);

    Winged *owner();

    WVertex *begin() const;
    WVertex *end() const;
    void begin(WVertex *v);
    void end(WVertex *v);

    WFace *left() const;
    WFace *right() const;
    void left(WFace *f);
    void right(WFace *f);

    WEdge *succLeft() const;
    WEdge *succRight() const;
    WEdge *predLeft() const;
    WEdge *predRight() const;

    void succLeft(WEdge *e);
    void succRight(WEdge *e);
    void predLeft(WEdge *e);
    void predRight(WEdge *e);

    void draw();

    void computeVector();
    void computePointEdge();

    const p3d::Vector3 &vec() const;
    p3d::Vector3 direction();
    const p3d::Vector3 &pointEdge() const;
    p3d::Vector3 midPoint();

    void insertVertexPointEdge();

    void mark();
    void unmark();
    bool isMark();

    void succLine(WEdge *e);
    WEdge *succLine();

    void followBoundary();
    void drawLine();

    friend std::ostream& operator <<(std::ostream &s,const WEdge &e);



  protected:
  private:
  WVertex *_begin;
  WVertex *_end;
  WFace *_left;
  WFace *_right;
  WEdge *_succL,*_succR,*_predL,*_predR;

  bool _mark;

  WEdge *_succLine;

  Winged *_owner;

  p3d::Vector3 _vec;
  p3d::Vector3 _pointEdge;

  unsigned int _index;
};



#endif // WEDGE_H

