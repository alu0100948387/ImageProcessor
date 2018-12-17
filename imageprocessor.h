#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QToolBar>

extern QVector<QPoint> points_;
extern float gamma_value;
extern double brightness_old, contrast_old;
extern double brightness_new, contrast_new;

#include "image.h"
#include "histogram.h"
#include "linear.h"
#include "gamma.h"
#include "brightness.h"

const int value_size = 256;

namespace Ui {
class ImageProcessor;
}

class ImageProcessor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageProcessor(QWidget *parent = nullptr);
    ~ImageProcessor();
    void set_linear_operation();
    void set_gamma();
    void set_brightness();

private slots:
    void newWindow(const QString &ImageName);
    void newWindow(QImage);
    void showCoordinates();
    void readyToolbar();
    void confirmedLinear() { set_linear_operation();}
    void check_ci_not_null();
    void confirmedGamma() { set_gamma();}
    void confirmedBrightness() { set_brightness(); }

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionHistogram_triggered();
    void on_actionConvert_to_Mono_triggered();
    void on_actionSet_intervals_triggered();
    void on_actionGamma_triggered();
    void on_actionEqualizate_triggered();
    void on_actionSpecification_triggered();
    void on_actionChange_brightness_triggered();
/*
    void on_actionSubstract_triggered();
    void on_actionAdd_triggered();
    void on_actionMultiply_triggered();
    void on_actionDivide_triggered();
    void on_actionMin_triggered();
    void on_actionMax_triggered();
    void on_actionImage_calculator_triggered();
    void on_actionThreashold_triggered();
*/

    /*MATH SLOTS */
    void on_actionImage_calculator_triggered();
    void on_actionThreshold_triggered();
    void on_actionAdd_triggered();
    void on_actionSubstract_triggered();
    void on_actionMultiply_triggered();
    void on_actionDivide_triggered();
    void on_actionMin_triggered();
    void on_actionMax_triggered();

    void add_images(const QImage &img1, const QImage &img2);
    void sub_images(const QImage &img1, const QImage &img2);
    void mult_images(const QImage &img1, const QImage &img2);
    void div_images(const QImage &img1, const QImage &img2);
    void diff_images(const QImage &img1, const QImage &img2);

    /* GEOMETRICAL SLOTS */
    void on_actionHorizontal_Mirror_triggered();

    void on_actionVertical_Mirror_triggered();

    void on_actionTransposed_Image_triggered();

    void on_actionScale_triggered();

    void on_actionRotate_triggered();

private:
    Ui::ImageProcessor *ui;
    Image* current_image;
    QString extension;

    // Need it for image calculator
    QVector<Image*> opened_images;
    int image_counter;
};

#endif // IMAGEPROCESSOR_H
