#include "Mesh.h"

#include "MeshGL.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Tools.h"
#include "Matrix4.h"
#include <fstream>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/

using namespace std;
using namespace p3d;

Mesh::~Mesh() {
  delete _render;
}

Mesh::Mesh() {
  _positionMesh.clear();
  _normalMesh.clear();
  _texCoordMesh.clear();

  _vertexAttrib.clear();

  _normalFace.clear();
  _render=new MeshGL(this);
}


Mesh *Mesh::clone() {
  Mesh *res=new Mesh();
  MeshGL *g=res->render();
  *res=*this;
  res->render(g);
  return res;
}


void Mesh::initDraw() {
  _render->initBuffer();
}

void Mesh::draw() {
  _render->draw();
}

void Mesh::drawNormal(double l) {
  _render->drawNormal(l);
}

void Mesh::check() {
  cout << "nb position = " << _positionMesh.size() << endl;
  cout << "nb faces = " << _vertexAttrib.size() << endl;
  for(auto i=_vertexAttrib.begin();i!=_vertexAttrib.end();i++) {
    cout << "(";
    VertexAttribFace::iterator j=(*i).begin();
    cout << "v " << j->_positionIndex << "n " << j->_normalIndex;
    j++;
    cout << ")" << endl;
  }
}

void Mesh::readInit(const string &resourceName,bool triangle) {
  read(resourceName);
  if (triangle) triangulate();
  scaleInBoxMin(-1,1,-1,1,-1,1);
  if (_normalMesh.empty()) {cout << "no normal in OBJ file => per vertex average is computed" << endl;computeNormal();}
  if (_texCoordMesh.empty()) {cout << "no texCoord in OBJ file => set to (0,0)" << endl;computeTexCoord();}
}


void Mesh::read(const string &resourceName) {
  QFileInfo resource=p3d::resourceFile(resourceName);
  fstream file;
  file.open(resource.filePath().toStdString().c_str(),ios::in);
  file.width(20);
  if (!file.is_open()) {
    throw Error("cant load file "+resource.filePath().toStdString(),__LINE__,__FILE__);
  }

  char s[200]; // dummy to read non-interpreted line
  char read[30];
  char c;
  double x,y,z;
  unsigned int indexPosition,indexTexture,indexNormal;
  VertexAttribFace face;
  VertexAttrib vertexAttrib;

  bool readingFace=false;
  while (!file.eof()) {
    file.clear();
    file >> read;
    if (string(read).compare("v")==0) {
      file >> x >> y >> z;
      _positionMesh.push_back(Vector3(x,y,z));
      continue;
    }
    if (string(read).compare("vn")==0) {
      file >> x >> y >> z;
      _normalMesh.push_back(Vector3(x,y,z));
      continue;
    }
    if (string(read).compare("vt")==0) {
      file >> x >> y;
      _texCoordMesh.push_back(Vector2(x,y));
      continue;
    }

    if (string(read).compare("f")==0) {
      face.clear();
      readingFace=true;
      while (readingFace) {
        file >> indexPosition;
        if (!file.fail()) {
          vertexAttrib._positionIndex=indexPosition-1; // starts at index 1 in obj file so -1 for internal arrays
          file >> c;
          if (!file.fail()) {
            if (c=='/') {
              file >> indexTexture;
              if (file.fail()) {
                file.clear();
              }
              else vertexAttrib._texCoordIndex=indexTexture-1;
              file >> c;
              if (!file.fail()) {
                if (c=='/') {
                  file >> indexNormal;
                  vertexAttrib._normalIndex=indexNormal-1;
                }
                else file.putback(c);
              }
            }
            else file.putback(c);
          }
          face.push_back(vertexAttrib);
        }
        else {
          readingFace=false;
        }
      }
      _vertexAttrib.push_back(face);
      continue;
    }
    file.getline(s,200);
  }
  file.close();
}

void Mesh::addPositionMesh(const p3d::Vector3 &p) {
  _positionMesh.push_back(p);
}

void Mesh::addFaceMesh(const std::vector<int> &indexList) {
  VertexAttribFace f;
  for(int i:indexList) {
    VertexAttrib v;
    v._positionIndex=i;
    f.push_back(v);
  }
  _vertexAttrib.push_back(f);
}


void Mesh::computeNormalFace(unsigned int i) {
  Vector3 s1;
  Vector3 s2;
  Vector3 s3;
  Vector3 n;

  double dist=0;

  unsigned int v1=0;
  unsigned int v2=1;
  unsigned int v3=2;
  bool stop=false;
  bool normalOk=false;
  while (!normalOk && !stop) {
    s1=positionVertexFace(i,v1);
    s2=positionVertexFace(i,v2);
    s3=positionVertexFace(i,v3);
    Vector3 u1(s1,s2);
    Vector3 u2(s2,s3);
    n.setCross(u1,u2);
    dist=n.length();
    if (dist>1e-05) {
      normalOk=true;
    }
    else {
      v3++;
      if (v3==nbVertexFace(i)) {
        v2++;
        v3=v2+1;
        if (v3==nbVertexFace(i)) {
          v1++;
          v2=v1+1;
          v3=v2+1;
        }
      }
      if (v3==nbVertexFace(i)) {
        stop=true;
//        throw Error("Normal problem",__LINE__,__FILE__);
      }
    }
  }

  if (stop) {
    n.set(0.0,0.0,0.0);
  }
  else  n.scale(1.0/dist);
  _normalFace[i]=n;
}

void Mesh::computeNormalFace() {
  _normalFace.resize(nbFace());
  for(unsigned int i=0;i<nbFace();++i) {
    computeNormalFace(i);
  }
}


void Mesh::computeNormal() {
  computeNormalFace();
  _normalMesh.resize(nbPosition());
  vector<unsigned int> nbFaceVertex; //nbFaceVertex[i] = nbFace around vertex i
  nbFaceVertex.resize(nbPosition());
  for(unsigned int i=0;i<nbPosition();++i) {
    _normalMesh[i].set(0,0,0);
    nbFaceVertex[i]=0;
  }
  for(unsigned int i=0;i<nbFace();++i) {
    for(unsigned int j=0;j<nbVertexFace(i);++j) {
      _vertexAttrib[i][j]._normalIndex=_vertexAttrib[i][j]._positionIndex;
    }
  }
  for(unsigned int i=0;i<nbFace();++i) {
    for(unsigned int j=0;j<nbVertexFace(i);++j) {
//      cout << "normal face = " << _normalFace[i] << endl;
      _normalMesh[_vertexAttrib[i][j]._normalIndex]+=_normalFace[i];
      nbFaceVertex[_vertexAttrib[i][j]._normalIndex]++;
    }
  }
  for(unsigned int i=0;i<nbNormal();++i) {
    _normalMesh[i]/=nbFaceVertex[i];
//    cout << "normal[i] = " << _normal[i] << endl;
  }
}

void Mesh::computeTexCoord() {
  _texCoordMesh.push_back(Vector2(0,0));
  for(unsigned int i=0;i<nbFace();++i) {
    for(unsigned int j=0;j<nbVertexFace(i);++j) {
      _vertexAttrib[i][j]._texCoordIndex=0;
    }
  }
}

void Mesh::scaleInBox(double left,double right,double bottom,double top,double znear,double zfar) {
  Vector3 mini(_positionMesh[0]);
  Vector3 maxi(_positionMesh[0]);

  for(unsigned int i=1;i<nbPosition();++i) {
    mini.setMinCoordinate(_positionMesh[i]);
    maxi.setMaxCoordinate(_positionMesh[i]);
  }
  for(unsigned int i=0;i<nbPosition();++i) {
    _positionMesh[i].x((_positionMesh[i].x()-mini.x())/(maxi.x()-mini.x())*(right-left)+left);
    _positionMesh[i].y((_positionMesh[i].y()-mini.y())/(maxi.y()-mini.y())*(top-bottom)+bottom);
    _positionMesh[i].z((_positionMesh[i].z()-mini.z())/(maxi.z()-mini.z())*(zfar-znear)+znear);
  }
}

void Mesh::scaleInBoxMin(double left,double right,double bottom,double top,double znear,double zfar) {
  Vector3 mini(_positionMesh[0]);
  Vector3 maxi(_positionMesh[0]);

  for(unsigned int i=1;i<_positionMesh.size();i++) {
    mini.setMinCoordinate(_positionMesh[i]);
    maxi.setMaxCoordinate(_positionMesh[i]);
  }

  Vector3 diag(mini,maxi);
  unsigned int which;
  double scale=diag.max(&which);
  scale=Vector3(right-left,top-bottom,zfar-znear)(which)/scale;


  for(unsigned int i=0;i<nbPosition();i++) {
    _positionMesh[i].x((_positionMesh[i].x()-mini.x())*scale+left+((right-left)-(maxi.x()-mini.x())*scale)/2.0);
    _positionMesh[i].y((_positionMesh[i].y()-mini.y())*scale+bottom+((top-bottom)-(maxi.y()-mini.y())*scale)/2.0);
    _positionMesh[i].z((_positionMesh[i].z()-mini.z())*scale+znear+((zfar-znear)-(maxi.z()-mini.z())*scale)/2.0);
  }
}

void Mesh::triangulate() {
  unsigned int nb=nbFace();
  for(unsigned int i=0;i<nb;i++) {
    if (nbVertexFace(i)>3) {
      VertexAttribFace add;
      for(unsigned int j=0;j<nbVertexFace(i)-3;++j) {
        add.clear();
        add.push_back(_vertexAttrib[i][0]);
        add.push_back(_vertexAttrib[i][j+2]);
        add.push_back(_vertexAttrib[i][j+3]);
        _vertexAttrib.push_back(add);
      }

      _vertexAttrib[i].erase(_vertexAttrib[i].begin()+3,_vertexAttrib[i].end());
    }
  }
}

void Mesh::rotateY(double angle) {
  Matrix4 trans;
  trans.setIdentity();
//  trans.translate((_aabbMax+_aabbMin)/2.0);
  trans.rotate(angle,Vector3(0,1,0));
//  trans.translate(-(_aabbMax+_aabbMin)/2.0);
  for(unsigned int i=0;i<_positionMesh.size();++i) {
    trans.transformPoint(&_positionMesh[i]);
  }

  for(unsigned int i=0;i<nbNormal();++i) {
    trans.transformDirection(_normalMesh[i]); // incorrect in general case (eg scale).
  }
}

void Mesh::requestInitDraw() {
   _render->_requestInitBuffer=true;
}

