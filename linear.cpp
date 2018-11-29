#include "linear.h"

QVector<QPoint> points_;

Linear::Linear()
{
    setWindowTitle(tr("Set Linear Operations..."));

    QLineEdit* size = new QLineEdit();
    confirm = new QPushButton(tr("Confirm"));
    form = new QFormLayout();
    layout = new QVBoxLayout();
    form->addRow(tr("Set number of points:"), size);


    connect(confirm, SIGNAL(clicked(bool)), this, SLOT(confirmed()));

    layout->addLayout(form);
    layout->addWidget(confirm);

    setLayout(layout);

}

void Linear::add_points()
{ window_mode = false;

    //lineEdits = form->findChildren<QLineEdit*>();

    QWidget *widget = form->itemAt(1)->widget();
    //points_size = widget->text().toInt();

    points_size = dynamic_cast <QLineEdit*> (widget)->text().toInt();


    // ... create complicated layout ...

    // completely delete layout and sublayouts
    QLayoutItem * item;
    QLayout * sublayout;
    /*while ((item = layout->itemAt(0))) {
        if ((sublayout = layout->layout()) != 0) {
            while ((item = sublayout->itemAt(0))) {
                if ((widget = sublayout->widget()) != 0) {widget->hide(); delete widget;}
                else {delete item;}

                delete sublayout;
            }
        }
        else if ((widget = layout->widget()) != 0) {widget->hide(); delete widget;}
        else {delete item;}
    }

    delete layout;*/

    while (QLayoutItem* item = layout->takeAt(0))
        {
            if (QWidget* widget = item->widget())
                    widget->deleteLater();
            if (QLayout* childLayout = item->layout())
              {while (QLayoutItem* item = childLayout->takeAt(0))
                    {
                        if (QWidget* widget = item->widget())
                                widget->deleteLater();
                        delete item;
                    }
              }
            delete item;
        }

    delete layout;
    //layout->update();


    form = new QFormLayout();
    layout = new QVBoxLayout();
    confirm = new QPushButton();

    connect(confirm, SIGNAL(clicked(bool)), this, SLOT(confirmed()));

    //setLayout(layout);

  /* create coordinate widgets */
  for (int i=0; i<points_size; i++)
  {
    QHBoxLayout* hBoxLayout = new QHBoxLayout();

    QLineEdit* x = new QLineEdit();
    QLabel* label_y = new QLabel(tr("Y:"));
    QLineEdit* y = new QLineEdit();

    hBoxLayout->addWidget(x);
    hBoxLayout->addWidget(label_y);
    hBoxLayout->addWidget(y);

    QWidget* container = new QWidget();
    container->setLayout(hBoxLayout);

    form->addRow(tr("X:"), container);
  }

    layout->addLayout(form);

    layout->addWidget(confirm);

    setLayout(layout);
}

void Linear::createVector()
{
    points_.resize(points_size);

    for(int i=0; i<points_size; i++)
    {
       /* for (int j=0; j< form->itemAt(i*2 + 1)->widget()->layout()->count(); j++)
          if (qobject_cast<QLineEdit*>(form->itemAt(i*2 + 1)->widget()->layout()->itemAt(i)->widget()) != NULL)
            int c = dynamic_cast <QLineEdit*>(form->itemAt(i*4 + 1)->widget()->layout()->itemAt(i))->text().toInt();
        */

        (points_)[i].setX(dynamic_cast <QLineEdit*>(form->itemAt(i*2+ 1)->widget()->layout()->itemAt(0)->widget())->text().toInt());
        (points_)[i].setY(dynamic_cast <QLineEdit*>(form->itemAt(i*2+ 1)->widget()->layout()->itemAt(2)->widget())->text().toInt());
    }

    destroy();
}

