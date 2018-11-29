#ifndef GAMMA_H
#define GAMMA_H

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class Gamma: public QWidget
{
    Q_OBJECT

public slots:
    void confirmed();

public:
    Gamma();

signals:
    void buttonClk();

private:
    QFormLayout* form;
    QVBoxLayout* layout;
    QPushButton* confirm;
};

#endif // GAMMA_H
