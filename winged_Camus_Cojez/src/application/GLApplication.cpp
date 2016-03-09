#include "GLApplication.h"
#include "GLRender.h"
#include "GLTool.h"
#include <iostream>

/**

  @author F. Aubert
  **/

using namespace std;
using namespace p3d;

GLApplication::~GLApplication() {
  delete[] _wingedObject;
}

GLApplication::GLApplication() {

  _leftPanelMenu << "Draw OBJ" << "WFace::draw (wire)" << "Show/Hide Vertex Normal" << "WFace::draw (light)" << "Boundary" << "Next Object";


  _camera.frustum(-0.01,0.01,-0.01,0.01,_frustum,1000);
  _camera.position(Vector3(0,0,10));
  _camera.lookAt(Vector3(0,0,0));

  _nbObject=3;
  _wingedObject=new Winged[3];

  _visuPoint=false;
  _visuBoundary=false;
  _visuHide=0;
  _computeVertexNormal=false;
  _drawLineCatmull=false;
  _boundary.clear();
  _active=0;

  _wingedObject[0].read("002_cube.obj");
  _wingedObject[1].read("cow.obj");
  _wingedObject[2].read("al.obj");

  _visuNormal=false;

}




/** ********************************************************************** **/
void GLApplication::initialize() {
  // appelée 1 seule fois à l'initialisation du contexte
  // => initialisations OpenGL

  glClearColor(1,1,1,1);

//  glLineWidth(4);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1);

  initGLTool();

  _ground.readMipmap("mosaic_pierre.jpg");

  for(unsigned int i=0;i<_nbObject;++i) {
    _wingedObject[i].initLineAttrib();
  }

}

void GLApplication::resize(int width,int height) {
  // appelée à chaque dimensionnement du widget OpenGL
  // (inclus l'ouverture de la fenêtre)
  // => réglages liés à la taille de la fenêtre
  glViewport(0,0,width,height);
  _camera.viewport(0,0,width,height);
}

void GLApplication::update() {
  // appelée toutes les 20ms (60Hz)
  // => mettre à jour les données de l'application
  // avant l'affichage de la prochaine image (animation)
  // ...

  updateCamera();

  switch(keyPressed()) {
  case Qt::Key_C:
    _wingedObject[_active].catmullClark();
    _wingedObject[_active].computeNormalVertex();
    _wingedObject[_active].stats();
    break;
  case Qt::Key_V:
    _wingedObject[_active].computePointFace();

    _wingedObject[_active].computePointEdge();

    _wingedObject[_active].computePointVertex();

    break;
  case Qt::Key_P:
    _visuPoint=!_visuPoint;
    break;
  case Qt::Key_B:
    _visuBoundary=!_visuBoundary;
    if (_visuBoundary) _boundary=_wingedObject[_active].boundary();

    break;
  case Qt::Key_N:
    _wingedObject[_active].computeNormalVertex();
    break;
  case Qt::Key_G:
    _visuHide=(_visuHide+1)%3;
    break;
  case Qt::Key_H:
    _drawLineCatmull=!_drawLineCatmull;
    break;
  case Qt::Key_Less:
    _active=(_active+1)%_nbObject;
    _visuBoundary=false;
    break;
  case Qt::Key_Greater:
    if (_active==0) _active=_nbObject-1;
    else _active=(_active-1);
    _visuBoundary=false;
    break;
  }
}

void GLApplication::draw() {
  // appelée après chaque update
  // => tracer toute l'image
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  p3d::apply(_camera);

  drawGround();


  if (_visuBoundary) {
    glLineWidth(4);
    p3d::ambientColor=Vector4(1,0,0);
    shaderVertexAmbient();
    for(unsigned int i=0; i<_boundary.size(); i++) {
      _boundary[i]->drawLine();
    }
  }

  drawWinged();

  glUseProgram(0);


  snapshot(); // capture opengl window if requested
}

/** ********************************************************************** **/
/** i = button number, s = button text
 */
void GLApplication::leftPanel(int i,const std::string &s) {
//  cout << "GLApplication : button clicked " << i << " " << s << endl;

  switch (i) {
    case 0:case 1:
      _visuHide=i;
      break;
    case 2:
      _visuNormal=!_visuNormal;_wingedObject[_active].computeNormalVertex();
      break;
    case 3:
      _visuHide=2;
      _wingedObject[_active].computeNormalVertex();
      break;
    case 4:
      _visuBoundary=!_visuBoundary;
      if (_visuBoundary) _boundary=_wingedObject[_active].boundary();
      break;
    case 5:
      _active=(_active+1)%3;
      if (_visuHide==2 || _visuNormal) {
        _wingedObject[_active].computeNormalVertex();
      }
      break;
    default:break;
  }

}



/** *********************************************************** **/

void GLApplication::drawGround() {
  p3d::modelviewMatrix.push();
  p3d::textureMatrix.push();
  p3d::textureMatrix.scale(135,135,135);
  p3d::modelviewMatrix.translate(0,-3,0);
  p3d::modelviewMatrix.scale(10,10,10);
  p3d::drawGround(_ground);
  p3d::textureMatrix.pop();
  p3d::modelviewMatrix.pop();
}

void GLApplication::updateCamera() {
  if (mouseLeft()) {
//    _camera.lookAt(Vector3(0,0,0));
    Vector3 t=_camera.pointTo(Coordinate_Local,Vector3(0,0,0));
    Vector3 vertical=_camera.directionTo(Coordinate_Local,Vector3(0,1,0));
    _camera.translate(t,Coordinate_Local);
    _camera.rotate(-deltaMouseX(),vertical,Coordinate_Local);
    _camera.rotate(deltaMouseY(),Vector3(1,0,0),Coordinate_Local);
    _camera.translate(-t,Coordinate_Local);
  }
  if (accelerateWheel()) {
    _frustum*=1.05;
    _camera.frustum(-0.01,0.01,-0.01,0.01,_frustum,1000);
  }
  if (decelerateWheel()) {
    _frustum/=1.05;
    _camera.frustum(-0.01,0.01,-0.01,0.01,_frustum,1000);
  }
  if (left()) _camera.left(0.2);
  if (right()) _camera.right(0.2);
  if (forward()) _camera.forward(0.2);
  if (backward()) _camera.backward(0.2);
}








/** ******************************************************************* **/
/**
  Drawings
**/


void GLApplication::drawWinged() {
  p3d::materialBlueGreen();
  glLineWidth(2);
  if (_drawLineCatmull) {
    _wingedObject[_active].drawLineCatmull();
  }
  else {
    switch(_visuHide) {
    case 0:
      p3d::shaderVertexAmbient();
      _wingedObject[_active].drawWire(Vector4(0,0,0));
      break;
    case 1:
      p3d::ambientColor=Vector4(0,0.4,0.9,1);
      p3d::shaderVertexAmbient();
      _wingedObject[_active].draw(false);
      break;
    case 2:
      p3d::shaderLightPhong();
      _wingedObject[_active].draw(true);
      break;
    }
  }
  if (_visuPoint) {
    p3d::ambientColor=Vector4(0,0,1,1);
    _wingedObject[_active].drawPointFace();
    p3d::ambientColor=Vector4(1,0,0,1);
    _wingedObject[_active].drawPointEdge();
    p3d::ambientColor=Vector4(0,1,0,1);
    _wingedObject[_active].drawPointVertex();
  }
  if (_visuNormal) {
    _wingedObject[_active].drawNormal();
  }
}



