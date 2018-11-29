#include "brightness.h"

double brightness_new, contrast_new;
double brightness_old, contrast_old;

Brightness::Brightness(QImage* image)
{
    setWindowTitle(tr("Change Brightness & Contrast..."));

    confirm = new QPushButton();
    layout = new QVBoxLayout();
    form = new QFormLayout();

    connect(confirm, SIGNAL(clicked(bool)), this, SLOT(confirmed()));


    QHBoxLayout* hBoxLayout_new = new QHBoxLayout(this);
    QHBoxLayout* hBoxLayout_old = new QHBoxLayout(this);
    QWidget* container_old = new QWidget(this);
    QWidget* container_new = new QWidget(this);

    get_contrast(image, brightness_old, contrast_old);

    /* first row in GUI */

    QLineEdit* line_b_old = new QLineEdit(this);
    QLabel* label_c_old = new QLabel(tr("Current contrast:"));
    QLineEdit* line_c_old = new QLineEdit(this);

    line_b_old->setText(QString::number(brightness_old));
    line_c_old->setText(QString::number(contrast_old));
    line_c_old->setDisabled(true);
    line_b_old->setDisabled(true);

    hBoxLayout_old->addWidget(line_b_old);
    hBoxLayout_old->addWidget(label_c_old);
    hBoxLayout_old->addWidget(line_c_old);

    container_old->setLayout(hBoxLayout_old);

    form->addRow(tr("Current brightness:"), container_old);

    /* second row in GUI */

    QLineEdit* line_b_new = new QLineEdit(this);
    QLabel* label_c_new = new QLabel(tr("Wanted contrast:"));
    QLineEdit* line_c_new = new QLineEdit(this);

    hBoxLayout_new->addWidget(line_b_new);
    hBoxLayout_new->addWidget(label_c_new);
    hBoxLayout_new->addWidget(line_c_new);

    line_b_new->setText(QString::number(0));
    line_c_new->setText(QString::number(0));

    container_new->setLayout(hBoxLayout_new);

    form->addRow(tr("Wanted brightness:"), container_new);

    /* set the main layout */

    layout->addLayout(form);
    layout->addWidget(confirm);
    setLayout(layout);

}


void Brightness::get_contrast(QImage* image,  double& brightness, double& contrast)
{
    unsigned long int sum = 0;
    unsigned long int sum2 = 0;

    /* get histogram */
    for(int i=0; i<image->width(); i++)
       { for(int j=0; j<image->height(); j++)
         {
          sum += QColor(image->pixel(i,j)).lightness();
          sum2 += pow(QColor(image->pixel(i,j)).lightness(),2);
         }
       }

    brightness = sum / (image->width() * image->height());
    //contrast = (pow(sum,2) /( image->width() * image->height())) - pow(brightness, 2);
    contrast = sum2 /( image->width() * image->height()) - pow(brightness, 2);
    contrast = sqrt(contrast);
}

void Brightness::confirmed()
{
    brightness_new = dynamic_cast<QLineEdit*>(form->itemAt(3)->widget()->layout()->itemAt(0)->widget())->text().toFloat();
    contrast_new = dynamic_cast<QLineEdit*>(form->itemAt(3)->widget()->layout()->itemAt(2)->widget())->text().toFloat();
    emit buttonClk();
    destroy();
}
