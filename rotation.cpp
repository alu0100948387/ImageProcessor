#include <imageprocessor.h>

#include <math.h>

void ImageProcessor::on_actionRotate_triggered()
{
    float degree;

    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    { QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    QLineEdit *degree_edit = new QLineEdit(&dialog);
    QString degree_menu = QString("Rotación en grados a la derecha:");
    form.addRow(degree_menu, degree_edit);

    QRadioButton* ropin = new QRadioButton(tr("Rotar + pintar"), &dialog);

    QRadioButton* neigh = new QRadioButton(tr("Vecinos mas próximos"), &dialog);
    QRadioButton* bilin = new QRadioButton(tr("Bilineal"), &dialog);

    form.addRow(ropin);

    neigh->setChecked(true);

    form.addRow( neigh, bilin);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        degree = degree_edit->text().toFloat();
    }
            /* for both operations we need the operated image's size*/

    float Ex, Ey, Fx, Fy, Gx, Gy, Hx, Hy;
    float min_x, max_x, min_y, max_y;
    int new_image_size_x, new_image_size_y;

    float radians =degree * M_PI/180;

    // E = (0,0)
    Ex = 0 * cos(radians) - 0 * sin(radians);
    Ey = 0 * sin(radians) + 0 * cos(radians);
    // F = (0, height)
    Fx = 0 * cos(radians) - (current_image->get_image()->height()-1) * sin(radians);
    Fy = 0 * sin(radians) + (current_image->get_image()->height()-1) * cos(radians);
    // G = (width, height)
     Gx = (current_image->get_image()->width()-1) * cos(radians) - (current_image->get_image()->height()-1) * sin(radians);
     Gy = (current_image->get_image()->width()-1) * sin(radians) + (current_image->get_image()->height()-1) * cos(radians);
    // H = (width, 0)
     Hx = (current_image->get_image()->width()-1)  * cos(radians) - 0 * sin(radians);
     Hy = (current_image->get_image()->width()-1)  * sin(radians) + 0 * cos(radians);

     min_x = std::min(Ex, Fx);
            min_x = std::min (min_x, Gx);
            min_x = std::min (min_x, Hx);
     min_y = std::min(Ey, Fy);
            min_y = std::min(min_y, Gy);
            min_y = std::min(min_y, Hy);
     max_x = std::max(Ex, Fx);
            max_x = std::max (max_x, Gx);
            max_x = std::max (max_x, Hx);
     max_y = std::max(Ey, Fy);
            max_y = std::max(max_y, Gy);
            max_y = std::max(max_y, Hy);

     // Image proportion in defect (not in excess)
    new_image_size_x = round (max_x - min_x);
    new_image_size_y = round (max_y - min_y);

    QImage image(new_image_size_x, new_image_size_y, QImage::Format_ARGB32);

    if(ropin->isChecked())
     {
       for(int i=0; i<(*current_image).get_image()->width(); i++)
        for(int j=0; j<(*current_image).get_image()->height(); j++)
          {
            // Using direct transformation, as we paint each pixel of source image into their position in the result image

             int X = round( i * cos(radians) -  j * sin(radians)  - min_x);
             int Y = round( i * sin(radians) + j * cos(radians)  - min_y);

             int saturation = QColor((*current_image).get_image()->pixel(i,j)).saturation();
             int hue = QColor((*current_image).get_image()->pixel(i,j)).hue();
             int lightness = QColor((*current_image).get_image()->pixel(i,j)).lightness();

             image.setPixel(X,Y, QColor::fromHsl(hue, saturation , lightness).rgb());
         }
    }
    else if (neigh->isChecked())
    {
        for(int i=0; i<image.width(); i++)
         for(int j=0; j<image.height(); j++)
           {
             // obtenemos la transformacion inversa
             float x_original_image =  (i + min_x) * cos(radians) + (j + min_y) * sin(radians);
             float y_original_image = - ( (i + min_x) * sin(radians)) +  (j + min_y) * cos(radians);

              int X = round(x_original_image);
              int Y = round(y_original_image);

              if ((X < (*current_image).get_image()->width() ) & (Y < (*current_image).get_image()->height()) & (X >= 0) & (Y >= 0))
               {
                    int saturation = QColor((*current_image).get_image()->pixel(X,Y)).saturation();
                    int hue = QColor((*current_image).get_image()->pixel(X,Y)).hue();
                    int lightness = QColor((*current_image).get_image()->pixel(X,Y)).lightness();

                    image.setPixel(i,j, QColor::fromHsl(hue, saturation , lightness).rgb());
               }
            }

    }
    else
    {
        for(int i=0; i<image.width(); i++)
         for(int j=0; j<image.height(); j++)
           {
             // obtenemos los cuatros puntos
             float x =  (i + min_x) * cos(radians) + (j + min_y) * sin(radians);
             float y = - ( (i + min_x) * sin(radians)) +  (j + min_y) * cos(radians);

             int X_ant = int(x);
             int Y_ant = int(y);
             int X_pos = X_ant+1;
             int Y_pos = Y_ant+1;

             int p = x - X_ant;
             int q = y - Y_ant;


             if ((X_pos < (*current_image).get_image()->width() ) & (Y_pos < (*current_image).get_image()->height()) & (X_ant >= 0) & (Y_ant >= 0))
              {
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
    }

    newWindow(image);
    }
}
