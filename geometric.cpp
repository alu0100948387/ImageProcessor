#include <imageprocessor.h>


void ImageProcessor::on_actionHorizontal_Mirror_triggered()
{


    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {

        /* build new image */
        QImage image = *current_image->get_image();

        for(int i=0; i<image.width(); i++)
                for(int j=0; j<image.height(); j++)
                {
                    int saturation = QColor((*current_image).get_image()->pixel(image.width()-1-i,j)).saturation();
                    int hue = QColor((*current_image).get_image()->pixel(image.width()-1-i,j)).hue();
                    int lightness = QColor((*current_image).get_image()->pixel(image.width()-1-i,j)).lightness();

                    image.setPixel(i,j, QColor::fromHsl(hue, saturation , lightness).rgb());
                }

         newWindow(image);

    }
}

void ImageProcessor::on_actionVertical_Mirror_triggered()
{

    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {

        /* build new image */
        QImage image = *current_image->get_image();

        for(int i=0; i<image.width(); i++)
                for(int j=0; j<image.height(); j++)
                {
                    int saturation = QColor((*current_image).get_image()->pixel(i,image.height()-1-j)).saturation();
                    int hue = QColor((*current_image).get_image()->pixel(i,image.height()-1-j)).hue();
                    int lightness = QColor((*current_image).get_image()->pixel(i,image.height()-1-j)).lightness();

                    image.setPixel(i,j, QColor::fromHsl(hue, saturation , lightness).rgb());
                }

         newWindow(image);

    }
}

void ImageProcessor::on_actionTransposed_Image_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {

        /* build new image */
        QImage image(current_image->get_image()->height(),current_image->get_image()->width(),QImage::Format_ARGB32);


        for(int i=0; i<current_image->get_image()->width(); i++)
                for(int j=0; j<current_image->get_image()->height(); j++)
                {
                    int saturation = QColor((*current_image).get_image()->pixel(j,i)).saturation();
                    int hue = QColor((*current_image).get_image()->pixel(j,i)).hue();
                    int lightness = QColor((*current_image).get_image()->pixel(j,i)).lightness();

                    image.setPixel(i,j, QColor::fromHsl(hue, saturation , lightness).rgb());
                }

        // Necesario para que el nombre sea Nombre + índice
        image.setText("Name:", current_image->get_image()->text("Name:"));

         newWindow(image);

    }
}


void ImageProcessor::on_actionScale_triggered()
{
    bool interpolation;
    float percentage_x, percentage_y;

    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    { QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);
    QLabel *x_label = new QLabel(tr(QString::number(current_image->get_image()->width()).toUtf8()),&dialog);
    QString x_menu = QString("X original:");
    form.addRow(x_menu, x_label);

    QLabel *y_label = new QLabel(tr(QString::number(current_image->get_image()->height()).toUtf8()),&dialog);
    QString y_menu = QString("Y original:");
    form.addRow(y_menu, y_label);

    QLineEdit *x_new_edit = new QLineEdit(&dialog);
    QString x_new_menu = QString("X nuevo:");
    form.addRow(x_new_menu, x_new_edit);

    QLineEdit *y_new_edit = new QLineEdit(&dialog);
    QString y_new_menu = QString("Y nuevo:");
    form.addRow(y_new_menu, y_new_edit);

    QLineEdit *x_percentage = new QLineEdit(&dialog);
    QString x_percentage_menu = QString("Porcentaje escala X:");
    form.addRow(x_percentage_menu, x_percentage);

    QLineEdit *y_percentage = new QLineEdit(&dialog);
    QString y_percentage_menu = QString("Porcentaje escala Y:");
    form.addRow(y_percentage_menu, y_percentage);

    QRadioButton* neigh = new QRadioButton(tr("Vecinos mas próximos"), &dialog);
    QRadioButton* bilin = new QRadioButton(tr("Bilineal"), &dialog);

    neigh->setChecked(true);

    form.addRow(neigh, bilin);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        if(!(x_new_edit->text().isEmpty()) && !(y_new_edit->text().isEmpty()))
        {
            percentage_x = x_new_edit->text().toFloat() / x_label->text().toFloat();
            percentage_y = y_new_edit->text().toFloat() / y_label->text().toFloat();
        }
        else if (!(x_percentage->text().isEmpty())&& !(y_percentage->text().isEmpty()))
        {
            percentage_x = x_percentage->text().toFloat();
            percentage_y = y_percentage->text().toFloat();
        }
        //else pop warning

        interpolation = !(neigh->isChecked());
    }



    //se realiza por defecto, no por exceso
    QImage image(round(current_image->get_image()->width()* percentage_x),round(current_image->get_image()->height()* percentage_y),QImage::Format_ARGB32);

    if(interpolation == PROX_NEIGHBOURS)
     {
       for(int i=0; i<image.width(); i++)
        for(int j=0; j<image.height(); j++)
          {
            // obtenemos la transformacion inversa
            float x_original_image = i / percentage_x;
            float y_original_image = j / percentage_y;

             int X = round(x_original_image);
             int Y = round(y_original_image);

             int saturation = QColor((*current_image).get_image()->pixel(X,Y)).saturation();
             int hue = QColor((*current_image).get_image()->pixel(X,Y)).hue();
             int lightness = QColor((*current_image).get_image()->pixel(X,Y)).lightness();

             image.setPixel(i,j, QColor::fromHsl(hue, saturation , lightness).rgb());
         }
    }

    else // bilineal transformation
    {
        for(int i=0; i<image.width(); i++)
         for(int j=0; j<image.height(); j++)
           {
             // obtenemos los cuatros puntos
             float x = i/percentage_x;
             float y = j/percentage_y;

             int X_ant = int(x);
             int Y_ant = int(y);
             int X_pos = X_ant+1;
             int Y_pos = Y_ant+1;

             int p = x - X_ant;
             int q = y - Y_ant;

             //obtenemos los valores de los cuatro puntos
             int A_red= QColor((*current_image).get_image()->pixel(X_ant,Y_pos)).red();
             int A_green= QColor((*current_image).get_image()->pixel(X_ant,Y_pos)).green();
             int A_blue= QColor((*current_image).get_image()->pixel(X_ant,Y_pos)).blue();

             int B_red= QColor((*current_image).get_image()->pixel(X_pos,Y_pos)).red();
             int B_green= QColor((*current_image).get_image()->pixel(X_pos,Y_pos)).green();
             int B_blue= QColor((*current_image).get_image()->pixel(X_pos,Y_pos)).blue();

             int C_red= QColor((*current_image).get_image()->pixel(X_ant,Y_ant)).red();
             int C_green= QColor((*current_image).get_image()->pixel(X_ant,Y_ant)).green();
             int C_blue= QColor((*current_image).get_image()->pixel(X_ant,Y_ant)).blue();

             int D_red= QColor((*current_image).get_image()->pixel(X_pos,Y_ant)).red();
             int D_green= QColor((*current_image).get_image()->pixel(X_pos,Y_ant)).green();
             int D_blue= QColor((*current_image).get_image()->pixel(X_pos,Y_ant)).blue();

              float red = C_red + (D_red + C_red)* p + (A_red - C_red)* q + (B_red + C_red - A_red - D_red) * p * q;
              float green = C_green + (D_green + C_green)* p + (A_green - C_green)* q + (B_green + C_green - A_green - D_green) * p * q;
              float blue = C_blue + (D_blue + C_blue)* p + (A_blue - C_blue)* q + (B_blue + C_blue - A_blue - D_blue) * p * q;

              image.setPixel(i,j, QColor(int(red), int(green) , int(blue)).rgb());
          }
    }
    newWindow(image);
    }
}


