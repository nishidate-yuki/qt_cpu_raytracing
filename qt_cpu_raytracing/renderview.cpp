#include "renderview.h"

constexpr int DEPTH = 32;

RenderView::RenderView(QWidget *parent)
    : QGraphicsView(parent)
{
    graphicsScene = new QGraphicsScene();
    setScene(graphicsScene);

    constexpr int width = 480;
    constexpr int height = 480;
    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    setMaximumSize(qMin(width+2, 1280), qMin(height+2, 720));

    // rendererをrenderThreadに移動
    renderer = new Renderer(width, height);
    renderer->moveToThread(&renderThread);

    // connect to Renderer
    connect(&renderThread, &QThread::finished, renderer, &QObject::deleteLater);
    connect(this, &RenderView::renderRequested, renderer, &Renderer::render);
    connect(renderer, &Renderer::finished, this, &RenderView::setImage);

    renderThread.start();
}

RenderView::~RenderView()
{
    renderThread.quit();
    renderThread.wait();
}

void RenderView::setImage(QImage image)
{
    qDebug() << "RenderView::setImage()";
    pixmapItem = graphicsScene->addPixmap(QPixmap::fromImage(image));
    pixmapItem->setPixmap(QPixmap::fromImage(image));
    setMinimumSize(qMin(image.width()+2, 1280), qMin(image.height()+2, 720));
    setMaximumSize(qMin(image.width()+2, 1280), qMin(image.height()+2, 720));
}
