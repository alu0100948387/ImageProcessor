#ifndef LINEAR_H
#define LINEAR_H

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

//QVector<QPoint> points_;

class Linear: public QWidget
{
    Q_OBJECT

public slots:
    void confirmed()
         { if (window_mode) add_points();
           else { createVector();
            emit buttonClk();
            }
        }

signals:
    void buttonClk();

public:
    Linear();
    //Linear(QWidget * parent = 0);
    void add_points();
    void createVector();

private:
    QFormLayout* form;
    QVBoxLayout* layout;
    QPushButton* confirm;

    int points_size = 1;
    bool window_mode = true;
};

#endif // LINEAR_H
