#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <math.h>

class Brightness: public QWidget
{
    Q_OBJECT

public slots:
    void confirmed();

signals:
    void buttonClk();

public:
    Brightness(QImage*);
    void get_contrast(QImage*, double&, double&);

private:
    QVBoxLayout* layout;
    QFormLayout* form;
    QPushButton* confirm;

    //float contrast_new = 0;
};

#endif // BRIGHTNESS_H
