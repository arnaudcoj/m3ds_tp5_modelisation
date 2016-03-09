#ifndef WFACE_H
#define WFACE_H

/**
@author F. Aubert
*/

class WFace;

#include "WEdge.h"
#include "Winged.h"
#include "Vector3.h"

class WFace
{
  public:
    WFace(Winged *prop);
    virtual ~WFace();

    void draw(bool withNormal);
    void drawLineCatmull();

    unsigned int index();
    void index(unsigned int i);

    Winged *owner();

    WEdge *edge();
    void edge(WEdge *e);

    const p3d::Vector3 &normal() const;
    void normal(const p3d::Vector3 &n);

    void computeNormal();

    void computePointFace();
    const p3d::Vector3 &pointFace() const;

    void insertVertexPointFace();

  protected:
  private:
  WEdge *_edge;

  p3d::Vector3 _normal;
  p3d::Vector3 _pointFace;

  Winged *_owner;

  unsigned int _index;
};

#endif // WFACE_H

