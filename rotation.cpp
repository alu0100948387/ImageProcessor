#include <imageprocessor.h>


void ImageProcessor::on_actionRotate_triggered()
{

    if (current_image == nullptr)
    {
        QMessageBox::warning(this, tr("My Application"),
                                       tr("No image selected.\n"),
                                       QMessageBox::Cancel);

    }else
    {
        bool ok, ok1, ok2;
        ok = false;
        ok1 = ok2 = ok;
        QString mode,item;
        mode = "Paint+Rotate";
        item = "None";
        double value = QInputDialog::getDouble(this, tr("Degrees (º)"),
                                     tr("Factor:"), 1.25, -360, 360, 2, &ok);


        if (ok)
        {
            QStringList items;
            items << tr("Paint+Rotate") << tr("Inverse");

            mode = QInputDialog::getItem(this, tr("Mode"),
                                                 tr("Mode:"), items, 0, false, &ok1);

            if (ok1 && mode == "Inverse")
            {
                QStringList items;
                items << tr("None") << tr("Bilineal") << tr("Bicubic");

                item = QInputDialog::getItem(this, tr("Interpolation"),
                                                     tr("Interpolation:"), items, 0, false, &ok2);


                if (ok2)
                {


                }




            }



            if (ok1 && mode == "Paint+Rotate")
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
                 newWindow(image);



        }




        }


    }

}
