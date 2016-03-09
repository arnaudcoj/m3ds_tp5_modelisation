#ifndef WINGEDMAP_H
#define WINGEDMAP_H

/**
@file
@author F. Aubert
@brief Vector3 operations (x,y,z)

*/

#include <map>
#include "Mesh.h"
#include "Winged.h"

class WingedMap
{
  public:
    WingedMap();
    virtual ~WingedMap();

    void read(const std::string &nom,Winged *winged);

    WVertex *wvertex(unsigned int i);

    WEdge *findEdge(WVertex *v1,WVertex *v2);

  protected:
  private:
  std::vector<WVertex *> _wvertex;

  std::vector<std::vector<WEdge *> > _vertexEdge;

  p3d::Mesh *_mesh;
};

#endif // WINGEDMAP_H

