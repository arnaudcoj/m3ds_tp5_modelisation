#ifndef WINGED_H
#define WINGED_H


/**
@file
@author F. Aubert
@brief  Winged-Edges : stockage et construteurs des entités d'un winged + opérations sur l'objet

*/

#include <vector>
#include "Vector3.h"
#include <string>

#include "WVertex.h"
#include "WEdge.h"
#include "WFace.h"

class Winged
{
  public:
    Winged();
    virtual ~Winged();

    void read(std::string nom);

    unsigned int nbVertex();
    unsigned int nbFace();
    unsigned int nbEdge();

    void eraseFace(unsigned int i);
    void deleteFace(unsigned int i);

    WVertex *createVertex(const p3d::Vector3 &p);
    WFace *createFace();
    WEdge *createEdge(WVertex *v1,WVertex *v2);

    Winged *clone();

    WEdge *findEdge2(WVertex *v1,WVertex *v2);

    WVertex *vertex(int i);
    WFace *face(int i);
    WEdge *edge(int i);

    void index();

    void stats();


    void computeNormalFace();
    void computeNormalVertex();


    // methods for catmull-clark subdivision scheme
    void computePointFace();
    void computePointEdge();
    void computePointVertex();
    void catmullClark();

    void check();
    void checkVertexBound();
    void checkEdgeFree();

    void initLineAttrib();

    void draw(bool withNormal);
    void drawEdgeBoundary();
    void drawPointFace();
    void drawPointEdge();
    void drawPointVertex();
    void drawWire(const p3d::Vector4 &color);
    void drawEdgeBound(const p3d::Vector4 &color);
    void drawLineCatmull();
    void drawNormal();

    void markAll();
    void unmarkAll();

    std::vector<WEdge *> boundary();

  protected:
  private:
  std::vector<WEdge *> _edge;
  std::vector<WFace *> _face;
  std::vector<WVertex *> _vertex;

  std::vector<p3d::Vector3> _lineAttrib;
};

#endif // WINGED_H

