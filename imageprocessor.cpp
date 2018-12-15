#include "imageprocessor.h"
#include "ui_imageprocessor.h"

ImageProcessor::ImageProcessor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImageProcessor),
    image_counter(0)
{
    ui->setupUi(this);

    ui->menuBar->setNativeMenuBar(false);

    setWindowTitle(tr("Image Processor"));
    setMinimumSize(360, 100);
    setMaximumSize(60, 100);
    resize(360, 100);
    statusBar()->hide();
    readyToolbar();
}

ImageProcessor::~ImageProcessor()
{

    QVector<Image*>::iterator i;
    for( i = opened_images.begin();  i != opened_images.end(); i++)
    {
            delete *i;
            opened_images.erase(i); // Remove element from list.
            --i;
    }
    current_image=nullptr;
    delete current_image;
    delete ui;
}

void ImageProcessor::readyToolbar()
{
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->addAction(QIcon(":resources/icons/open.png"), "open", this, &ImageProcessor::on_actionOpen_triggered);

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":resources/icons/save.png"), "save");

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":/resources/icons/rectangle.png"), "rectangle", this, &ImageProcessor::check_ci_not_null);

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":/resources/icons/crop.png"), "crop", this, [=]
    {

        if (current_image != nullptr)
        {
            if  (current_image->get_rect() != nullptr)
            {
                QImage cropped_copy = current_image->get_image()->copy(*current_image->get_rect());
                newWindow(cropped_copy);
            }
        }

    });
}

void ImageProcessor::showCoordinates()
{
    qInfo() << "Reading mouse hover input.";
    QString text;
    //text = QString("%1 X %2").arg(m_event->pos().x()).arg(m_event->pos().y());

}

void ImageProcessor::check_ci_not_null()
{
    if (current_image != nullptr)
    {
        current_image->initialize_rect();
    }
}

void ImageProcessor::on_actionNew_triggered(){}

void ImageProcessor::on_actionOpen_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            "/home/sergio/Imágenes",
                            "Images (*.pgm *.bmp *.gif *.png *.xpm *.jpg *.jpeg);;All files (*)");

    if (!files.isEmpty()) {

        if (files.size() > 1)
        {
            for (int i=0; i < files.size(); ++i)
            {
                newWindow(files[i]);
            }
        }else
        {
            newWindow(files[0]);
        }

    }
}

void ImageProcessor::newWindow(const QString &ImageName)
{
    qInfo() << ImageName;
     Image* image =  new Image(ImageName, this);
 //    extension = QFileInfo(filename).suffix();

      connect(image, &Image::hovered, this, [this](double x, double y, QRgb value){
         qInfo() << Q_FUNC_INFO << QString("xpos %1, ypos %2, RGB: %3").arg(x).arg(y).arg(value);
         QString text;
         QColor rgb(value);
         text = QString("Coordinates %1 x %2 rgb: (%3, %4, %5)").arg(x).arg(y).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
          ui->coordinates->setText(text);
       });

     connect(image, &Image::clicked, this, [this]
     { current_image = static_cast<Image*>(sender());
     });

     connect(image, &Image::closing, this, [this](const int &id){

             QVector<Image*>::iterator i;
             for( i = opened_images.begin();  i != opened_images.end(); i++)
             {
                 if ((*i)->get_id() == id)
                 {
                     qInfo() << tr("Closing image with id: %1").arg(id);
                     delete *i;
                     opened_images.erase(i); // Remove element from list.
                     qInfo() << tr("Size of opened_images now: %1").arg(opened_images.size());
                     --i;
                 }
             }

             current_image=nullptr;
         });

         current_image = image;
         Image* ptr = image;

         image->set_id(image_counter);
         opened_images.append(ptr);
         ++image_counter;

         image->show();


}

void ImageProcessor::newWindow(QImage temp_image)
{
    Image* image =  new Image(temp_image, this, image_counter);

       connect(image, &Image::hovered, this, [this](double x, double y, QRgb value){
          qInfo() << Q_FUNC_INFO << QString("xpos %1, ypos %2, RGB: %3").arg(x).arg(y).arg(value);
          QString text;
          QColor rgb(value);
          text = QString("Coordinates %1 x %2 rgb: (%3, %4, %5)").arg(x).arg(y).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
           ui->coordinates->setText(text);
        });

       connect(image, &Image::clicked, this, [this]
       { current_image = static_cast<Image*>(sender());

       });

       connect(image, &Image::closing, this, [this](const int &id){

               QVector<Image*>::iterator i;
               for( i = opened_images.begin();  i != opened_images.end(); i++)
               {
                   if ((*i)->get_id() == id)
                   {
                       qInfo() << tr("Closing image with id: %1").arg(id);
                       delete *i;
                       opened_images.erase(i); // Remove element from list.
                       qInfo() << tr("Size of opened_images now: %1").arg(opened_images.size());
                       --i;
                   }
               }

               current_image=nullptr;
           });

           current_image = image;
           Image* ptr = image;

           image->set_id(image_counter);
           opened_images.append(ptr);
           ++image_counter;

           image->show();

}

void ImageProcessor::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save an image..."));
        if (filename != "")
        {
            //if(filename.endsWith(".*"))
            if(QFileInfo(filename).suffix() == "")
                filename = filename + "." + extension;

            QFile f (filename);
            if (!f.exists()) f.open(QIODevice::WriteOnly);

            if (!current_image->get_image()->save(filename))
               return;

            // else debug the error

        }
}

void ImageProcessor::on_actionHistogram_triggered()
{
    Histogram * h= new Histogram(current_image->get_image());
    h->show();

}

void ImageProcessor::on_actionConvert_to_Mono_triggered()
{
    float value; int int_value;
    QImage image = *current_image->get_image();

    if( !image.isGrayscale())
    { for(int i=0; i<image.width(); i++)
            for(int j=0; j<image.height(); j++)
            { /* PAL value */
                value = QColor(image.pixel(i,j)).red() * 0.222 + QColor(image.pixel(i,j)).green() * 0.707 + QColor(image.pixel(i,j)).blue() * 0.071;
                int_value = qRound(value);
                if(int_value > 255) int_value = 255;
                image.setPixel(i,j, QColor(int_value, int_value, int_value).rgb());
            }
        newWindow(image);
    }
}

void ImageProcessor::set_linear_operation()
{
    int pointer = 0;
    //create LUT
    QVector<int> LUT;
    QVector<QPoint> points = points_;

    if (points[0].x() > 0) points.push_front(QPoint(0,0));
    if (points[points.size()-1].x() < 255) points.push_back(QPoint(255,255));

    LUT.resize(value_size);

    for (int i=0; i<LUT.size(); i++) //0~255
    {
      if (i > points[pointer+1].x())  pointer++;

      float factor = ((i - points[pointer].x())*1.0 / (points[pointer+1].x() - points[pointer].x()));
      float value = points[pointer].y() + factor * (points[pointer+1].y() - points[pointer].y());
      LUT[i] = qRound(value);

    }


    /* create image */
    QImage image = *current_image->get_image();

    for(int i=0; i<image.width(); i++)
            for(int j=0; j<image.height(); j++)
            {
                int red = QColor(image.pixel(i,j)).red();
                int green = QColor(image.pixel(i,j)).green();
                int blue = QColor(image.pixel(i,j)).blue();
                int alpha = QColor(image.pixel(i,j)).alpha();
                image.setPixel(i,j, QColor(LUT[red], LUT[green], LUT[blue], alpha).rgb());
            }

        newWindow(image);

}

void ImageProcessor::on_actionSet_intervals_triggered()
{
    Linear *l = new Linear();
    l->show();

    connect(l, SIGNAL(buttonClk()), this, SLOT(confirmedLinear()));

}

void ImageProcessor::on_actionGamma_triggered()
{
    Gamma* g = new Gamma();
    g->show();

    connect(g, SIGNAL(buttonClk()), this, SLOT(confirmedGamma()));
}

void ImageProcessor::set_gamma()
{
    /* this gamma value must be positive
        * )  <1 is a logaritmic transformation
        * )  =1 no transformation
        * )  >1 is a exponential transformation
     */

    QVector<int> LUT;
    LUT.resize(value_size);

    for (int i=0; i<LUT.size(); i++)
        { float aux;
          /*normalize Vin to [0,1] */
          aux = i*1.0/(value_size-1);

          /*exponentiation of the value_size
             this value is also between [0,1]*/
          aux = pow(aux, gamma_value);

          /*then got back to [0,255 and round it */
          aux = aux * (value_size-1);
          LUT[i] = qRound(aux);

        }
    /* build new image */
    QImage image = *current_image->get_image();

    for(int i=0; i<image.width(); i++)
            for(int j=0; j<image.height(); j++)
            {
                int red = QColor(image.pixel(i,j)).red();
                int green = QColor(image.pixel(i,j)).green();
                int blue = QColor(image.pixel(i,j)).blue();
                int alpha = QColor(image.pixel(i,j)).alpha();
                image.setPixel(i,j, QColor(LUT[red], LUT[green], LUT[blue], alpha).rgb());
            }

     newWindow(image);
}

void ImageProcessor::on_actionEqualizate_triggered()
{
    QVector<int> count_vector, LUT;
    float aux;

    count_vector.resize(value_size);
    LUT.resize(value_size);

    /* get histogram */ 
    for(int i=0; i<current_image->get_image()->width(); i++)
       { for(int j=0; j<current_image->get_image()->height(); j++)
         {
            count_vector[QColor(current_image->get_image()->pixel(i,j)).toHsl().value()]+=1;
         }
       }

    /* build LUT for each color */
      unsigned int sum = 0;
      for(int j=0; j<LUT.size(); j++)
      {
          sum += count_vector[j];

          aux = (value_size-1)*1.0 * sum / (current_image->get_image()->height() * current_image->get_image()->width());

          LUT[j] = qRound(aux);
          //if(LUT[i][j] < 0) LUT[i][j] = 0;
      }

    /* build new image */
    QImage image = *current_image->get_image();


    for(int i=0; i<image.width(); i++)
            for(int j=0; j<image.height(); j++)
            {
                int saturation = QColor(image.pixel(i,j)).saturation();
                int hue = QColor(image.pixel(i,j)).hue();
                int value = QColor(image.pixel(i,j)).value();

                image.setPixel(i,j, QColor::fromHsv(hue, saturation, LUT[value]).rgb());
            }
     newWindow(image);
}

void ImageProcessor::on_actionSpecification_triggered()
{
    /* in this function we try to implement the HSL method instead of doing independent operations for each RGB*/

     QImage* aux_image;
     float sum_in = 0, sum_out = 0;
     int pointer_in = 0, pointer_out = 0;
     float epsilon = 0.00001;
     QVector<float> histogram_in, histogram_out;
     QVector<int> LUT;

     QString file = QFileDialog::getOpenFileName(
                             this,
                             "Select one to compare",
                             "/home/sergio/Imágenes",
                             "Images (*.pgm *.bmp *.gif *.png *.xpm *.jpg *.jpeg);;All files (*)");

     aux_image = new QImage();
     aux_image->load(file);


     //build both histograms
     histogram_in.resize(value_size);

     for(int i=0; i<current_image->get_image()->width(); i++)
        { for(int j=0; j<current_image->get_image()->height(); j++)
          {
             histogram_in[QColor(current_image->get_image()->pixel(i,j)).lightness()]+=1;
          }
        }

     histogram_out.resize(value_size);
     for(int i=0; i<aux_image->width(); i++)
        { for(int j=0; j<aux_image->height(); j++)
          {
             histogram_out[QColor(aux_image->pixel(i,j)).lightness()]+=1;
          }
        }

     /* normalize both histograms*/
     for(int k=0; k<value_size; k++)
     {
         histogram_in[k] = histogram_in[k] * 1.0 / (current_image->get_image()->width() * current_image->get_image()->height());
         histogram_out[k] = histogram_out[k]*1.0 / (aux_image->width() * aux_image->height());
     }

     /*compare both histograms using Vout = Co * Ci^-1 * Vin */

     LUT.resize(value_size);

     /*
     for(int i=0; i<value_size; i++)
     { int k = value_size-1;
         do {
             LUT[i] = k;
             k--;
         }while ((k>=0) && (histogram_in[i]<= histogram_out[k]));

     }
*/



     sum_in += histogram_in[0];
     sum_out += histogram_out[0];

     while((pointer_in< value_size-1) && (pointer_out < value_size-1))
     {
         if (sum_in > sum_out)
                 //(sum_in > (sum_out + epsilon))
         {
             pointer_out++;
             sum_out += histogram_out[pointer_out];
         }

         else
         {
             LUT[pointer_in] = pointer_out;
             pointer_in++;
             sum_in += histogram_in[pointer_in];
         }
     }

     while(pointer_in < value_size)
     {

         LUT[pointer_in] = pointer_in;
         pointer_in ++;
     }



     /* build new image */
     QImage image = *current_image->get_image();

     for(int i=0; i<image.width(); i++)
             for(int j=0; j<image.height(); j++)
             {
                 int saturation = QColor(image.pixel(i,j)).saturation();
                 int hue = QColor(image.pixel(i,j)).hue();
                 int lightness = QColor(image.pixel(i,j)).lightness();

                // if (lightness != LUT[lightness])
                 //    int l = 0;
                 image.setPixel(i,j, QColor::fromHsl(hue, saturation , LUT[lightness]).rgb());
             }

      newWindow(image);
}

void ImageProcessor::on_actionChange_brightness_triggered()
{
    Brightness* g = new Brightness(current_image->get_image());
    g->show();

    connect(g, SIGNAL(buttonClk()), this, SLOT(confirmedBrightness()));
}

void ImageProcessor::set_brightness()
{
    QVector<int> LUT;
    LUT.resize(value_size);

    double a, b; //following the y = ax + b

    a = contrast_new / contrast_old;
    b = brightness_new - (a * brightness_old);

    for (int i=0; i<value_size; i++)
      {  LUT[i] = qRound(a* i +b);
         if (LUT[i] < 0) LUT[i] = 0;
         else if (LUT[i] > 255) LUT[i] = 255;
      }

    /* build new image */
    QImage image = *current_image->get_image();

    for(int i=0; i<image.width(); i++)
            for(int j=0; j<image.height(); j++)
            {
                int saturation = QColor(image.pixel(i,j)).saturation();
                int hue = QColor(image.pixel(i,j)).hue();
                int lightness = QColor(image.pixel(i,j)).lightness();

                image.setPixel(i,j, QColor::fromHsl(hue, saturation , LUT[lightness]).rgb());
            }

    newWindow(image);
}

void ImageProcessor::on_actionImage_calculator_triggered()
{
    if (opened_images.size() < 2)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("Need 2 images opened.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        QDialog dialog(this);
        // Use a layout allowing to have a label next to each field
        QFormLayout form(&dialog);


        QComboBox *qb1 = new QComboBox(&dialog);
        for (int i=0; i < opened_images.size(); ++i)
        {
            qb1->addItem(opened_images.at(i)->get_image()->text("Name:"));
        }
        QString label = QString("Image 1:");
        form.addRow(label, qb1);


        QComboBox *qb2 = new QComboBox(&dialog);
        qb2->addItem("Add");
        qb2->addItem("Substract");
        qb2->addItem("Multiply");
        qb2->addItem("Divide");
        qb2->addItem("Difference");

        QString label2 = QString("Operation:");
        form.addRow(label2, qb2);


        QComboBox *qb3 = new QComboBox(&dialog);
        for (int i=0; i < opened_images.size(); ++i)
        {
            qb3->addItem(opened_images.at(i)->get_image()->text("Name:"));
        }
        QString label3 = QString("Image 2:");
        form.addRow(label3, qb3);

        // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);

        connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted)
        {
            int img1 = qb1->currentIndex();
            int op = qb2->currentIndex();
            int img2 = qb3->currentIndex();

            qInfo() << tr("Operación a realizar %1 con %2 y %3.\n").arg(op).arg(img1).arg(img2);

            switch(op)
            {
            case 0:
                    add_images(*opened_images[img1]->get_image(), *opened_images[img2]->get_image());
                break;

            case 1:
                    sub_images(*opened_images[img1]->get_image(), *opened_images[img2]->get_image());
                break;

            case 2:
                    mult_images(*opened_images[img1]->get_image(), *opened_images[img2]->get_image());
                break;

            case 3:
                    div_images(*opened_images[img1]->get_image(), *opened_images[img2]->get_image());
                break;

            case 4:
                    diff_images(*opened_images[img1]->get_image(), *opened_images[img2]->get_image());
                break;
            }
        }

    }
}


// Temporary objects are often created during execution of a C++ program. Result of C++ operators (unary, binary, logical, etc.) and return-by-value (int fun(){}) functions always give rise to temporary objects
// Need const because opened_images[x] returns a temporary, and temporary objects need to be passed as const references without being a const themselves.
// The idea is that a function taking a non-const reference parameter is stating that it wants to modify the parameter and allowing it to go back to the caller.
// Doing so with a temporary is meaningless and most likely an error.
void ImageProcessor::add_images(const QImage &img1, const QImage &img2)
{
        QImage copy1= img1;
        QImage copy2 = img2;

        int* r1 = new int();
        int* g1 = new int();
        int* b1 = new int();

        int* r2 = new int();
        int* g2 = new int();
        int* b2 = new int();

        int new_red = 0;
        int new_green = 0;
        int new_blue = 0;

        qInfo() << tr("Copy1 height: %1 width: %2").arg(copy1.height()).arg(copy1.width());
        qInfo() << tr("Copy2 height: %1 width: %2").arg(copy2.height()).arg(copy2.width());
        for (int y = 0; y < copy1.height(); y++) {

            QRgb *line1 = reinterpret_cast<QRgb *>(copy1.scanLine(y));
            QRgb *line2 = reinterpret_cast<QRgb *>(copy2.scanLine(y));

            for (int x = 0; x < copy1.width(); x++) {
                // line[x] has an individual pixel
                QColor pixel1 = line1[x];
                QColor pixel2 = line2[x];

                pixel1.getRgb(r1,g1,b1); // Obtain values RGB from pixel and points them with r,g,b pointers
                pixel2.getRgb(r2,g2,b2);

                //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r1).arg(*g1).arg(*b1);
                //qInfo() << text1;

                //QString text2 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r2).arg(*g2).arg(*b2);
                //qInfo() << text2;

                new_red = 0;
                new_green = 0;
                new_blue = 0;

                if ((*r1 + *r2 >= 255))
                    new_red = 255;
                else
                    new_red = *r1 + *r2;

                if ((*g1 + *g2 >= 255))
                    new_green = 255;
                else
                    new_green = *g1 + *g2;

                if ((*b1 + *b2 >= 255))
                    new_blue = 255;
                else
                    new_blue = *b1 + *b2;

                //QString text3 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                //qInfo() << text3;

                 line1[x] = QColor(new_red, new_green, new_blue).rgb();

            }
        }
        newWindow(copy1);
}

void ImageProcessor::sub_images(const QImage &img1, const QImage &img2)
{

    QImage copy1= img1;
    QImage copy2 = img2;

    int* r1 = new int();
    int* g1 = new int();
    int* b1 = new int();

    int* r2 = new int();
    int* g2 = new int();
    int* b2 = new int();

    int new_red = 0;
    int new_green = 0;
    int new_blue = 0;

    for (int y = 0; y < copy1.height(); y++) {
        QRgb *line1 = reinterpret_cast<QRgb *>(copy1.scanLine(y));
        QRgb *line2 = reinterpret_cast<QRgb *>(copy2.scanLine(y));

        for (int x = 0; x < copy1.width(); x++) {
            // line[x] has an individual pixel
            QColor pixel1 = line1[x];
            QColor pixel2 = line2[x];

            pixel1.getRgb(r1,g1,b1); // Obtain values RGB from pixel and points them with r,g,b pointers
            pixel2.getRgb(r2,g2,b2);

            QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r1).arg(*g1).arg(*b1);
            qInfo() << text1;

            new_red = 0;
            new_green = 0;
            new_blue = 0;

            // RED
            if (*r2 < *r1)
                new_red = *r1 - *r2;
            else
                new_red = 0;

            // GREEN
            if (*g2 < *g1)
                new_green = *g1 - *g2;
            else
                new_green = 0;

            // BLUE
            if (*b2 < *b1)
                new_blue = *b1 - *b2;
            else
                new_blue = 0;

            //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
            //qInfo() << text2;

            line1[x] = QColor(new_red, new_green, new_blue).rgb();
        }
    }
    newWindow(copy1);
}

void ImageProcessor::mult_images(const QImage &img1, const QImage &img2)
{

    QImage copy1= img1;
    QImage copy2 = img2;

    int* r1 = new int();
    int* g1 = new int();
    int* b1 = new int();

    int* r2 = new int();
    int* g2 = new int();
    int* b2 = new int();

    double new_red = 0;
    double new_green = 0;
    double new_blue = 0;

    for (int y = 0; y < copy1.height(); y++) {
        QRgb *line1 = reinterpret_cast<QRgb *>(copy1.scanLine(y));
        QRgb *line2 = reinterpret_cast<QRgb *>(copy2.scanLine(y));

        for (int x = 0; x < copy1.width(); x++) {
            // line[x] has an individual pixel
            QColor pixel1 = line1[x];
            QColor pixel2 = line2[x];

            pixel1.getRgb(r1,g1,b1); // Obtain values RGB from pixel and points them with r,g,b pointers
            pixel2.getRgb(r2,g2,b2);

            //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
            //qInfo() << text1;

            new_red = 0;
            new_green = 0;
            new_blue = 0;

            if ((*r1 * *r2 > 255))
                new_red = 255;
            else
                new_red = *r1 * *r2;

            if ((*g1 * *g2 > 255))
                new_green = 255;
            else
                new_green = *g1 * *g2;

            if ((*b1 * *b2 > 255))
                new_blue = 255;
            else
                new_blue = *b1 * *b2;

            //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
            //qInfo() << text2;

            line1[x] = QColor(new_red, new_green, new_blue).rgb();
        }
    }
    newWindow(copy1);
}

void ImageProcessor::div_images(const QImage &img1, const QImage &img2)
{

    QImage copy1= img1;
    QImage copy2 = img2;

    int* r1 = new int();
    int* g1 = new int();
    int* b1 = new int();

    int* r2 = new int();
    int* g2 = new int();
    int* b2 = new int();

    double new_red = 0;
    double new_green = 0;
    double new_blue = 0;

    for (int y = 0; y < copy1.height(); y++) {
        QRgb *line1 = reinterpret_cast<QRgb *>(copy1.scanLine(y));
        QRgb *line2 = reinterpret_cast<QRgb *>(copy2.scanLine(y));

        for (int x = 0; x < copy1.width(); x++) {
            // line[x] has an individual pixel
            QColor pixel1 = line1[x];
            QColor pixel2 = line2[x];

            pixel1.getRgb(r1,g1,b1); // Obtain values RGB from pixel and points them with r,g,b pointers
            pixel2.getRgb(r2,g2,b2);

            //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
            //qInfo() << text1;

            new_red = 0;
            new_green = 0;
            new_blue = 0;

            if (*r2 == 0)
                *r2 = 1;

            if (*g2 == 0)
                *g2 = 1;

            if (*b2 == 0)
                *b2 = 1;

            if ((*r1 / *r2 > 255))
                new_red = 255;
            else
                new_red = *r1 / *r2;

            if ((*g1 / *g2 > 255))
                new_green = 255;
            else
                new_green = *g1 / *g2;

            if ((*b1 / *b2 > 255))
                new_blue = 255;
            else
                new_blue = *b1 / *b2;

            //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
            //qInfo() << text2;

            line1[x] = QColor(new_red, new_green, new_blue).rgb();
        }
    }
    newWindow(copy1);
}

void ImageProcessor::diff_images(const QImage &img1, const QImage &img2)
{

    QImage copy1= img1;
    QImage copy2 = img2;

    int* r1 = new int();
    int* g1 = new int();
    int* b1 = new int();

    int* r2 = new int();
    int* g2 = new int();
    int* b2 = new int();

    int new_red = 0;
    int new_green = 0;
    int new_blue = 0;

    for (int y = 0; y < copy1.height(); y++) {
        QRgb *line1 = reinterpret_cast<QRgb *>(copy1.scanLine(y));
        QRgb *line2 = reinterpret_cast<QRgb *>(copy2.scanLine(y));

        for (int x = 0; x < copy1.width(); x++) {
            // line[x] has an individual pixel
            QColor pixel1 = line1[x];
            QColor pixel2 = line2[x];


            pixel1.getRgb(r1,g1,b1); // Obtain values RGB from pixel and points them with r,g,b pointers
            pixel2.getRgb(r2,g2,b2);

            //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
            //qInfo() << text1;

            new_red = 0;
            new_green = 0;
            new_blue = 0;

            // RED
            if (*r2 < *r1)
                new_red = *r1 - *r2;
            else
                new_red = 0;

            // GREEN
            if (*g2 < *g1)
                new_green = *g1 - *g2;
            else
                new_green = 0;

            // BLUE
            if (*b2 < *b1)
                new_blue = *b1 - *b2;
            else
                new_blue = 0;

            new_red = static_cast<int>(qFabs(*r1 - *r2));
            new_green = static_cast<int>(qFabs(*g1 - *g2));
            new_blue = static_cast<int>(qFabs(*b1 - *b2));

            //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
            //qInfo() << text2;

            line1[x] = QColor(new_red, new_green, new_blue).rgb();
        }
    }
    newWindow(copy1);
}

void ImageProcessor::on_actionThreshold_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        if (!current_image->get_image()->isGrayscale())
        {
            QMessageBox::warning(this, tr("My Application"),
                                           tr("Threshold only works in grayscale images.\n"
                                              "\n "
                                              "Your can use Image -> Converto to mono. \n"),
                                           QMessageBox::Ok);
        }else
        {

            QImage copy = *current_image->get_image();

            QDialog dialog(this);
            // Use a layout allowing to have a label next to each field
            QFormLayout form(&dialog);

            QLabel* info = new QLabel(&dialog);
            info->setText("Move the sliders to set threshold.\n");
            info->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            info->setWordWrap(true);

            form.addWidget(info);

            QLabel* min = new QLabel(&dialog);
            QLabel* max = new QLabel(&dialog);

            min->setText("0");
            max->setText("0");

            QSlider *slider_min = new QSlider(Qt::Horizontal, &dialog);
            slider_min->setMaximum(255);
            slider_min->setMinimum(0);
            slider_min->setTickInterval(1);

            connect(slider_min, &QSlider::valueChanged, this, [=]{
                min->setText(tr("%1").arg(slider_min->value()));
            });

            form.addRow(min);
            form.addRow(slider_min);

            QSlider *slider_max = new QSlider(Qt::Horizontal, &dialog);
            slider_max->setMaximum(255);
            slider_max->setMinimum(0);
            slider_max->setTickInterval(1);
            connect(slider_max, &QSlider::valueChanged, this, [=]{
                max->setText(tr("%1").arg(slider_max->value()));
            });

            form.addRow(max);
            form.addRow(slider_max);

            // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                       Qt::Horizontal, &dialog);
            form.addRow(&buttonBox);

            connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            if (dialog.exec() == QDialog::Accepted)
            {
                int min = slider_min->value();
                int max = slider_max->value();

                for (int y = 0; y < copy.height(); y++) {
                    QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                    for (int x = 0; x < copy.width(); x++) {
                        // line[x] has an individual pixel
                        QColor pixel = line[x];
                        int* r = new int();
                        int* g = new int();
                        int* b = new int();
                        pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                        //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                        //qInfo() << text1;


                        if ((*r < min) || (*r > max) || (*g < min) || (*g > max) || (*b < min) || (*b > max))
                            line[x] = QColor(255, 0, 0).rgb();
                        else
                            line[x] = QColor(*r, *g, *b).rgb();

                        //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                        //qInfo() << text2;

                    }
                }
                newWindow(copy);
            }
        }
    }
}

void ImageProcessor::on_actionAdd_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok;
        int value = QInputDialog::getInt(this, tr("Add"),
                                     tr("Value to add:"), 0, 0, 255, 1, &ok);
        if (ok)
        {
            QImage copy = *current_image->get_image();
            for (int y = 0; y < copy.height(); y++) {
                QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                for (int x = 0; x < copy.width(); x++) {
                    // line[x] has an individual pixel
                    QColor pixel = line[x];
                    int* r = new int();
                    int* g = new int();
                    int* b = new int();
                    pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                    //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                    //qInfo() << text1;

                    int new_red = 0;
                    int new_green = 0;
                    int new_blue = 0;

                    if ((*r + value > 255))
                        new_red = 255;
                    else
                        new_red = *r + value;

                    if ((*g + value > 255))
                        new_green = 255;
                    else
                        new_green = *g + value;

                    if ((*b + value > 255))
                        new_blue = 255;
                    else
                        new_blue = *b + value;

                    //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                    //qInfo() << text2;

                    line[x] = QColor(new_red, new_green, new_blue).rgb();
                }
            }
            newWindow(copy);
        }
    }
}


void ImageProcessor::on_actionSubstract_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok;
        int value = QInputDialog::getInt(this, tr("Substract"),
                                     tr("Value to substract:"), 0, 0, 255, 1, &ok);
        if (ok)
        {
            QImage copy = *current_image->get_image();
            for (int y = 0; y < copy.height(); y++) {
                QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                for (int x = 0; x < copy.width(); x++) {
                    // line[x] has an individual pixel
                    QColor pixel = line[x];
                    int* r = new int();
                    int* g = new int();
                    int* b = new int();
                    pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                    QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                    qInfo() << text1;

                    int new_red = 0;
                    int new_green = 0;
                    int new_blue = 0;

                    // RED
                    if (value < *r)
                        new_red = *r - value;
                    else
                        new_red = 0;


                    // GREEN
                    if (value < *g)
                        new_green = *g - value;
                    else
                        new_green = 0;

                    // BLUE
                    if (value < *b)
                        new_blue = *b - value;
                    else
                        new_blue = 0;


                    //new_red = static_cast<int>(qFabs(*r - value));
                    //new_green = static_cast<int>(qFabs(*g - value));
                    //new_blue = static_cast<int>(qFabs(*b - value));

                    QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                    qInfo() << text2;

                    line[x] = QColor(new_red, new_green, new_blue).rgb();
                }
            }
            newWindow(copy);
        }
    }
}


void ImageProcessor::on_actionMultiply_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok;
        double value = QInputDialog::getDouble(this, tr("Multiply"),
                                     tr("Factor:"), 1.25, 0, 255, 2, &ok);
        if (ok)
        {
            QImage copy = *current_image->get_image();
            for (int y = 0; y < copy.height(); y++) {
                QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                for (int x = 0; x < copy.width(); x++) {
                    // line[x] has an individual pixel
                    QColor pixel = line[x];
                    int* r = new int();
                    int* g = new int();
                    int* b = new int();
                    pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                    //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                    //qInfo() << text1;

                    double new_red = 0;
                    double new_green = 0;
                    double new_blue = 0;

                    if ((*r * value > 255))
                        new_red = 255;
                    else
                        new_red = *r * value;

                    if ((*g * value > 255))
                        new_green = 255;
                    else
                        new_green = *g * value;

                    if ((*b * value > 255))
                        new_blue = 255;
                    else
                        new_blue = *b * value;

                    //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                    //qInfo() << text2;

                    line[x] = QColor(static_cast<int>(new_red), static_cast<int>(new_green), static_cast<int>(new_blue)).rgb();
                }
            }
            newWindow(copy);
        }
    }
}

void ImageProcessor::on_actionDivide_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok;
        double value = QInputDialog::getDouble(this, tr("Divide"),
                                     tr("Factor:"), 1.25, 0.25, 255, 2, &ok);
        if (ok)
        {
            QImage copy = *current_image->get_image();
            for (int y = 0; y < copy.height(); y++) {
                QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                for (int x = 0; x < copy.width(); x++) {
                    // line[x] has an individual pixel
                    QColor pixel = line[x];
                    int* r = new int();
                    int* g = new int();
                    int* b = new int();
                    pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                    //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                    //qInfo() << text1;

                    double new_red = 0;
                    double new_green = 0;
                    double new_blue = 0;

                    if ((*r / value > 255))
                        new_red = 255;
                    else
                        new_red = *r / value;

                    if ((*g / value > 255))
                        new_green = 255;
                    else
                        new_green = *g / value;

                    if ((*b / value > 255))
                        new_blue = 255;
                    else
                        new_blue = *b / value;

                    //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                    //qInfo() << text2;

                    line[x] = QColor(static_cast<int>(new_red), static_cast<int>(new_green), static_cast<int>(new_blue)).rgb();
                }
            }
            newWindow(copy);
        }
    }
}

void ImageProcessor::on_actionMin_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok;
        int value = QInputDialog::getInt(this, tr("Min"),
                                     tr("Set minimum value:"), 0, 0, 255, 1, &ok);
        if (ok)
        {
            QImage copy = *current_image->get_image();
            for (int y = 0; y < copy.height(); y++) {
                QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                for (int x = 0; x < copy.width(); x++) {
                    // line[x] has an individual pixel
                    QColor pixel = line[x];
                    int* r = new int();
                    int* g = new int();
                    int* b = new int();
                    pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                    //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                    //qInfo() << text1;

                    int new_red = 0;
                    int new_green = 0;
                    int new_blue = 0;

                    if ((*r < value))
                        new_red = value;
                    else
                        new_red = *r;

                    if ((*g < value))
                        new_green = value;
                    else
                        new_green = *g;

                    if ((*b < value))
                        new_blue = value;
                    else
                        new_blue = *b;

                    //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                    //qInfo() << text2;

                    line[x] = QColor(new_red, new_green, new_blue).rgb();
                }
            }
            newWindow(copy);
        }
    }
}

void ImageProcessor::on_actionMax_triggered()
{
    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok;
        int value = QInputDialog::getInt(this, tr("Max"),
                                     tr("Set maximum value:"), 0, 0, 255, 1, &ok);
        if (ok)
        {
            QImage copy = *current_image->get_image();
            for (int y = 0; y < copy.height(); y++) {
                QRgb *line = reinterpret_cast<QRgb *>(copy.scanLine(y));
                for (int x = 0; x < copy.width(); x++) {
                    // line[x] has an individual pixel
                    QColor pixel = line[x];
                    int* r = new int();
                    int* g = new int();
                    int* b = new int();
                    pixel.getRgb(r,g,b); // Obtain values RGB from pixel and points them with r,g,b pointers

                    //QString text1 = QString("Before -> red: %1 green: %2 blue: %3").arg(*r).arg(*g).arg(*b);
                    //qInfo() << text1;

                    int new_red = 0;
                    int new_green = 0;
                    int new_blue = 0;

                    if ((*r > value))
                        new_red = value;
                    else
                        new_red = *r;

                    if ((*g > value))
                        new_green = value;
                    else
                        new_green = *g;

                    if ((*b > value))
                        new_blue = value;
                    else
                        new_blue = *b;

                    //QString text2 = QString("After -> red: %1 green: %2 blue: %3").arg(new_red).arg(new_green).arg(new_blue);
                    //qInfo() << text2;

                    line[x] = QColor(new_red, new_green, new_blue).rgb();
                }
            }
            newWindow(copy);
        }
    }
}


