
#include <QImage> // Pixel-Pixel image processing
#include <QLabel> // Image display
#include <QDebug>
#include <QtWidgets>
#include <QHoverEvent>
#include <QRect>
#include <QPainter>
#include <QRubberBand>


#include <QCloseEvent>

class Image : public QLabel
{
    Q_OBJECT

signals:
    void hovered(double x, double y, QRgb value);
    void rect_ready(QRect rect);
    void clicked();
    void closing(const int &id);

private:
    QImage *image_;
    QRect *rect_;
    bool rect_pressed_;
    QRubberBand *rubberband_;
    QRubberBand *selection_;
    QPoint origin_;

    int id_;

public:

    Image(QWidget *parent):
        image_(new QImage),
        rect_(nullptr),
        rect_pressed_(false),
        rubberband_(nullptr),
        selection_(nullptr),
        id_(0)
    {
        this->setParent(parent);
        this->setWindowFlag(Qt::Window);
    }

    Image(QImage image, QWidget *parent, const int& id):
        image_(new QImage(image)),
        rect_(nullptr),
        rect_pressed_(false),
        rubberband_(nullptr),
        selection_(nullptr),
        id_(id)
    {
        this->setParent(parent);
        this->setWindowFlag(Qt::Window);

        this->setMouseTracking(true);

        this->setPixmap(QPixmap::fromImage(*image_));

        // Change name adding to the parent name the id of this son
        QString name = tr("%1%2").arg(image_->text("Name:")).arg(id);
        image_->setText("Name:", name);

        image_->setText(tr("Parent Path:"), image_->text("Path:"));
        image_->setText(tr("Path:"), "");

        this->setMaximumSize(image_->size());
        this->setMinimumSize(image_->size());
        this->setWindowTitle(image_->text("Name:"));
        this->resize(image_->size());

    }

    Image(const QString &ImageName,  QWidget *parent):
        image_(new QImage(ImageName)),
        rect_(nullptr),
        rect_pressed_(false),
        rubberband_(nullptr),
        selection_(nullptr),
        id_(0)
    {
        this->setParent(parent);
        this->setWindowFlag(Qt::Window);

        this->setMouseTracking(true);

        this->setPixmap(QPixmap::fromImage(*image_));


        image_->setText(tr("Name:"), QFileInfo(ImageName).baseName());
        image_->setText(tr("Suffix:"), QFileInfo(ImageName).suffix());
        image_->setText(tr("Path:"), ImageName);

        this->setMaximumSize(image_->size());
        this->setMinimumSize(image_->size());
        this->setWindowTitle(image_->text("Name:"));
        this->resize(image_->size());

    }

    // Getters
    QImage* get_image(){return image_;}
    QImage* get_image() const {return image_;} // Returns const
    QRect* get_rect(){return rect_;}
    int get_id(){return id_;}

    // Setters
    void set_id(const int &id){ id_ = id;}


    void initialize_rect()
    {
        qInfo() << "Rect pressed";
        rect_ = new QRect;
        rect_pressed_ = true;
    }

    // Events

    void mouseMoveEvent(QMouseEvent *e){

        qInfo() << e->type();

        if (rubberband_)
            rubberband_->setGeometry(QRect(origin_, e->pos()).normalized());

        double xpos = e->pos().x();
        double ypos = e->pos().y();
        emit hovered(xpos, ypos, image_->pixel(xpos,ypos));

    }

    void mousePressEvent(QMouseEvent *e)
    {
        qInfo() << e->type();
        emit clicked();

        if (selection_ != nullptr)
        {
            selection_->hide();
            selection_ = nullptr;
            rect_ = nullptr;
            rect_pressed_ = false;
        }

        origin_ = e->pos();
        if (rubberband_ == nullptr)
        {
            rubberband_ = new QRubberBand(QRubberBand::Rectangle, this);
        }

        rubberband_->setGeometry(QRect(origin_, QSize()));
        rubberband_->show();

    }

    void mouseReleaseEvent(QMouseEvent *e)
    {
        qInfo() << e->type();

        if(rect_pressed_)
        {
            qDebug() << "Rect Pressed";
            *rect_ = rubberband_->geometry();
            selection_ = new QRubberBand(QRubberBand::Rectangle, this);
            selection_->setGeometry(*rect_);
            selection_->show();
            rect_pressed_ = false;
        }

        if (rubberband_)
            rubberband_->hide();
    }

    void closeEvent (QCloseEvent *event)
    {
     emit closing(id_);
    }


    // Operators
    bool operator!=(const Image& b)
    {
        if (id_ != b.id_)
            return true;
        else
            return false;
    }

    bool operator==(const Image& b)
    {
        if (id_ == b.id_)
            return true;
        else
            return false;
    }

    /*
    Image& copy(const Image*& b, const int &id)
    {
        if (this != b)
        {
            *image_ = *b->get_image(); // we get the QImage that returns get_image
            id_ = id;

        }
        return *this;

    }*/

};
