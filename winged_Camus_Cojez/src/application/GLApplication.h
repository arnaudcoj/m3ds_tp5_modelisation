#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H

/**

  @author F. Aubert
  **/


#include "GLWidget.h"
#include "Winged.h"
#include "Texture.h"
#include "Camera.h"

#include <string>
#include <QSize>

class GLApplication : public GLWidget {
  Q_OBJECT
public:
  GLApplication();
  virtual ~GLApplication();


  /** ***** **/
  /** GLApplication must implement these methods : */
  virtual void initialize();
  virtual void update();
  virtual void draw();
  virtual void resize(int width,int height);
  /** ***** **/

  virtual void leftPanel(int i, const std::string &s);
  QSize sizeHint() const {return QSize(500,500);}

private:
  void updateCamera();
  void drawWinged();
  void drawGround();


  unsigned int _nbObject;
  Winged *_wingedObject;

  bool _visuPoint;
  bool _visuBoundary;
  bool _visuNormal;
  unsigned int _visuHide;
  bool _computeVertexNormal;
  bool _drawLineCatmull;

  std::vector<WEdge *> _boundary;

  unsigned int _active;

  float _frustum=0.01;
  p3d::Texture _ground;
  p3d::Camera _camera;


};

#endif // GLApplication_H

