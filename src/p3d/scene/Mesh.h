#ifndef Mesh_H
#define Mesh_H

#include <vector>
#include <string>
#include "Object3D.h"
#include "Vector2.h"
#include "Vector3.h"


/*!
*
* @file Simple OBJ Loader :
* all faces are in the array vertexIndexFace
* each face (type VertexFace) is an array of VertexAttrib (one VertexAttrib per vertex of the face).
* VertexAttrib contains 3 indices (position, normal, texCoord) that refers to the following arrays :
* - positionObj : array of all positions directly read from the file .obj (i.e. the v)
* - normalObj : array of all normals read from the file .obj (i.e. the vn)
* - texCoordObj : array of all texture coordinates from the file .obj (i.e. the t).
*
* @brief
* @author F. Aubert
*
*/

namespace p3d {
class Vector2;
class MeshGL;

/*
 *
 * @class VertexAttrib contains all attributes of a vertex : each attribute is an index (refers to _positionMesh, _normalMesh and _texCoordMesh
 *
 */
class VertexAttrib {
public:
  int _positionIndex,_normalIndex,_texCoordIndex;
  VertexAttrib() : _positionIndex(-1),_normalIndex(-1),_texCoordIndex(-1) {}
};

/*
 *
 * @class VertexAttribFace : each face contains the attributes of all vertices
 *
 */

typedef std::vector<VertexAttrib> VertexAttribFace;

// modulo operator (% gives the remainder)
static inline int cycle(int i,int nb) {
    return (i%nb+nb)%nb;
}


class Mesh {
public:
    Mesh();
    virtual ~Mesh();

    void read(const std::string &filename);
    void readInit(const std::string &resourceName, bool triangle=true);
    void scaleInBox(double left, double right, double top, double bottom,double znear,double zfar);
    void scaleInBoxMin(double left, double right, double bottom, double top, double znear, double zfar);

    void check();
    void computeNormal();
    void computeTexCoord();
    void computeNormalFace();
    void computeNormalFace(unsigned int i);
    void triangulate();

    inline const Vector3 &positionMesh(unsigned int i) const {return _positionMesh[i];}
    inline const Vector3 &normalMesh(unsigned int i) const {return _normalMesh[i];}
    inline const Vector2 &texCoordMesh(unsigned int i) const {return _texCoordMesh[i];}

    inline const Vector3 &normalFace(unsigned int i) const {return _normalFace[i];}

    inline const VertexAttrib &vertexAttrib(unsigned int i,unsigned int j) const {return _vertexAttrib[i][cycle(j,nbVertexFace(i))];}
    inline int indexPositionVertexFace(unsigned int i,unsigned int j) const {return vertexAttrib(i,j)._positionIndex;}
    inline const Vector3 &positionVertexFace(unsigned int i,unsigned int j) const {return positionMesh(indexPositionVertexFace(i,j));}
    inline const Vector3 &normalVertexFace(unsigned int i,unsigned int j) const {return normalMesh(vertexAttrib(i,j)._normalIndex);}
    inline const Vector2 &texCoordVertexFace(unsigned int i,unsigned int j) const {return texCoordMesh(vertexAttrib(i,j)._texCoordIndex);}

    inline unsigned int nbPosition() const {return _positionMesh.size();}
    inline unsigned int nbNormal() const {return _normalMesh.size();}
    inline unsigned int nbTexCoord() const {return _texCoordMesh.size();}
    inline unsigned int nbVertexFace(unsigned int i) const {return _vertexAttrib[i].size();}
    inline unsigned int nbFace() const {return _vertexAttrib.size();}

    void draw();
    void drawNormal(double l);
    void initDraw();

    MeshGL *render() {return _render;}
    void render(MeshGL *render) {_render=render;}

    void rotateY(double angle);
    void addPositionMesh(const p3d::Vector3 &p);
    void addFaceMesh(const std::vector<int> &indexList);
    Mesh *clone();

    void requestInitDraw();
protected: // protected only for easier coding of the answers (more visibility of the internal structure)
    std::vector<p3d::Vector3> _positionMesh; //! x,y,z of a vertex (read from OBJ)
    std::vector<p3d::Vector3> _normalMesh; //! x,y,z of a normal  (read from OBJ)
    std::vector<p3d::Vector2> _texCoordMesh; //! s,t of a texCoord (read from OBJ)
    std::vector<p3d::Vector3> _normalFace; //! x,y,z of a normal (one normal per face)
    std::vector<VertexAttribFace> _vertexAttrib; //! _vertexAttrib[i][j] returns the VertexAttrib of the j-ieme vertex of the i-ieme face


    MeshGL *_render;
};
}

#endif // Mesh_H

