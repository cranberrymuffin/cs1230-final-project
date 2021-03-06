#include "view.h"

#include "gl/GLDebug.h"
#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>
#include <scenegraph/SceneviewScene.h>

using namespace CS123::GL;

View::View(QWidget* parent)
    : QGLWidget(ViewFormat(), parent)
    , tick_counter(0)
    , m_time()
    , m_timer()
    , m_captureMouse(false)
    , m_currentScene(nullptr)
    , m_defaultPerspectiveCamera(new CamtransCamera())
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor
    if (m_captureMouse) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The update loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    firstFrame = true;
}

View::~View()
{
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    initializeScene();
    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    m_time.start();
    m_timer.start(1000 / 60);

    checkError();
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    checkError();
    glEnable(GL_CULL_FACE);
    checkError();
    glCullFace(GL_BACK);
    checkError();
    glFrontFace(GL_CCW);
    checkError();
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float ratio = static_cast<QGuiApplication*>(QCoreApplication::instance())->devicePixelRatio();
    glViewport(0, 0, width() * ratio, height() * ratio);
    m_defaultPerspectiveCamera->setAspectRatio(static_cast<float>(width()) / static_cast<float>(height()));
    m_currentScene->m_height = height();
    m_currentScene->m_width = width();
    m_currentScene->m_aspect = ratio;
    m_currentScene->render(m_defaultPerspectiveCamera.get());
    // TODO: Implement the demo rendering here
}

void View::resizeGL(int w, int h)
{
    float ratio = static_cast<QGuiApplication*>(QCoreApplication::instance())->devicePixelRatio();
    w = static_cast<int>(w / ratio);
    h = static_cast<int>(h / ratio);
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent* event)
{
}

void View::mouseMoveEvent(QMouseEvent* event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    if (m_captureMouse) {
        int deltaX = event->x() - width() / 2;
        int deltaY = event->y() - height() / 2;
        if (!deltaX && !deltaY)
            return;
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // TODO: Handle mouse movements here
    }
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
}

void View::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QApplication::quit();

    // TODO: Handle keyboard presses here
    if (event->key() == Qt::Key_Space) {
        pauseSim = !pauseSim;
    }
}

void View::keyReleaseEvent(QKeyEvent* event)
{
}

void View::initializeScene()
{
    m_currentScene = std::make_unique<SceneviewScene>();
}

void View::tick()
{

    // Get the number of seconds since the last tick (variable update rate)

    if (firstFrame) {
        firstFrame = false;
        return;
    }
    if (!pauseSim) {
        float timeStep = 0.00026;
        int n = (0.0026 / timeStep);
        for (int i = 0; i < n; ++i)
            m_currentScene->update(timeStep);

        update();
        tick_counter = 0;
    }

}

void View::loadSceneviewSceneFromParser(CS123XmlSceneParser& parser)
{
    Scene::parse(m_currentScene.get(), &parser);
}

void View::settingsChanged()
{
    if (m_currentScene != nullptr) {
        // Just calling this function so that the scene is always updated.
        m_currentScene->settingsChanged();
    }
    update(); /* repaint the scene */
}

CamtransCamera* View::getCamtransCamera()
{
    return m_defaultPerspectiveCamera.get();
}
