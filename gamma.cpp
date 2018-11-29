#include "gamma.h"

float gamma_value;

Gamma::Gamma()
{
    /* create the interface of the Gamma Window*/
    setWindowTitle("Operation Gamma");

    QLineEdit* gamma_line_edit = new QLineEdit();

    confirm = new QPushButton(tr("Confirm"));
    form = new QFormLayout();
    layout = new QVBoxLayout();

    form->addRow(tr("Set the gamma value:"), gamma_line_edit);

    connect(confirm, SIGNAL(clicked(bool)), this, SLOT(confirmed()));

    layout->addLayout(form);
    layout->addWidget(confirm);

    setLayout(layout);

}

void Gamma::confirmed()
{
    gamma_value = dynamic_cast<QLineEdit*>(form->itemAt(1)->widget())->text().toFloat();
    emit buttonClk();
    destroy();
}
