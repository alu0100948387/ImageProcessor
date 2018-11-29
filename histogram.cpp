#include "histogram.h"

const unsigned int vector_size = 256;

Histogram::Histogram(QImage* image)
{
    width = image->width();
    height = image->height();

    textEdit = new QLabel;
    normal_button = new QRadioButton("Normal Count");
    log_button = new QRadioButton("Accumulative Count");

    connect(normal_button, SIGNAL(clicked()), this, SLOT(set_normal()));
    connect(log_button, SIGNAL(clicked()), this, SLOT(set_log()));

    setWindowTitle(tr("Histogram"));

    /* get histogram */
    count_vector.resize(4);
    for(int k=0; k< count_vector.size(); k++) count_vector[k].resize(vector_size);
    for(int i=0; i<width; i++)
       { for(int j=0; j<height; j++)
         {count_vector[RED][QColor(image->pixel(i,j)).red()] ++;
          count_vector[GREEN][QColor(image->pixel(i,j)).green()] ++;
          count_vector[BLUE][QColor(image->pixel(i,j)).blue()] ++;
          count_vector[MONO][qRound(static_cast<float>((QColor(image->pixel(i,j)).red() * 0.222 +
                                           QColor(image->pixel(i,j)).green() * 0.707 +
                                           QColor(image->pixel(i,j)).blue() * 0.071)))] ++;
         }
       }


    layout = new QVBoxLayout;
    build_graph();
    get_graph_data();

    QHBoxLayout* child_layout = new QHBoxLayout();

    child_layout->addWidget(textEdit);

    QHBoxLayout* rlayout = new QHBoxLayout();
    rlayout->addWidget(normal_button);
    rlayout->addWidget(log_button);

    normal_button->setChecked(true);


    if (!image->isGrayscale())
    {
        QVBoxLayout* rgb_layout = new QVBoxLayout();
        QPushButton* red_button = new QPushButton(tr("Red Histogram"));
        QPushButton* green_button = new QPushButton(tr("Green Histogram"));
        QPushButton* blue_button = new QPushButton(tr("Blue Histogram"));
        QPushButton* mono_button = new QPushButton(tr("Monocolor Histogram [Using PAL]"));

        connect(red_button, SIGNAL(pressed()), this, SLOT(set_red()));
        connect(green_button, SIGNAL(pressed()), this, SLOT(set_green()));
        connect(blue_button, SIGNAL(pressed()), this, SLOT(set_blue()));
         connect(mono_button, SIGNAL(pressed()), this, SLOT(set_mono()));

        rgb_layout->addWidget(red_button);
        rgb_layout->addWidget(green_button);
        rgb_layout->addWidget(blue_button);
        rgb_layout->addWidget(mono_button);
        rgb_layout->addLayout(rlayout);

        child_layout->addLayout(rgb_layout);
    }
    else
    {
        child_layout->addLayout(rlayout);
    }

    layout->addLayout(child_layout);
    setLayout(layout);


    this->setFixedSize(800,600);

}

void Histogram::build_graph()
{
    QBarSeries *series = new QBarSeries();
    QBarSet* set = new QBarSet("number");

    unsigned int sum = 0;

    for(int k=0; k<count_vector[graph_color].size(); k++)
       {//*series << QPoint(k, count_vector[k]);
        if(graph_mode) *set << count_vector[graph_color][k];
        else
           { sum += count_vector[graph_color][k];
            //*set << log(count_vector[graph_color][k]);
            *set << sum;
           }
        set->setColor(Qt::black);
       }

      series->append(set);
      QChart *chart = new QChart();

      chart->addSeries(series);
      chartView = new QChartView(chart);

      chartView->setRenderHint(QPainter::Antialiasing);

      chart->legend()->hide();



      QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("Pixel value");
        axisX->setLabelFormat("%i");
        axisX->setTickCount(series->count());
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

       QValueAxis *axisY = new QValueAxis();
           axisY->setTitleText("Count");
           //axisY->setBase(1000.0);
           //axisY->setMinorTickCount(-1);
           chart->addAxis(axisY, Qt::AlignLeft);
          series->attachAxis(axisY);

      layout->addWidget(chartView,0,0);

      //chart.legend().markers(serie_without_marker)[0].setVisible(false);º
}

void Histogram::update_graph()
{
    unsigned int sum = 0;

    chartView->chart()->removeAllSeries();

    QBarSeries *series = new QBarSeries();
    QBarSet* set = new QBarSet("number");
    for(int k=0; k<count_vector[graph_color].size(); k++)
       {//*series << QPoint(k, count_vector[k]);
        if(graph_mode) *set << count_vector[graph_color][k];
        else
           { sum += count_vector[graph_color][k];
            //*set << log(1 + count_vector[graph_color][k]);
            *set << sum;
           }
        switch(graph_color)
         {case RED: set->setColor(Qt::red); break;
         case GREEN: set->setColor(Qt::green); break;
         case BLUE: set->setColor(Qt::cyan); break;
         case MONO: set->setColor(Qt::black); break;
         }
       }

    series->append(set);

    chartView->chart()->addSeries(series);
    get_graph_data();

}

void Histogram::get_graph_data()
{
    /* get the data such as brightness, contrast, max, min, median */
    QString s;
    const unsigned int half_samples = width * height / 2;
    float brightness, contrast;
    int max = 0, min = 255, median = 0;
    unsigned long int sum = 0; unsigned long int sum2 = 0;
    long double entropy = 0;
    //unsigned int count_min = INFINITY, count_max=0,
    unsigned int count_mean = 0;

    for(int i=0; i<count_vector[graph_color].size(); i++)
       { sum += (count_vector[graph_color][i] * i);
        for(int j=0; j<count_vector[graph_color][i]; j++)
          { sum2 +=pow( i ,2);
          }
        count_mean += count_vector[graph_color][i];
        if (count_vector[graph_color][i] != 0)
         {  if(i < min) min = i;
            if (i > max) max = i;
            entropy += ( (long double) count_vector[graph_color][i] / (width * height) ) * log2((long double)count_vector[graph_color][i] / (half_samples * 2) );
         }


        /*
         if ( count_vector[i] > count_max)
         {max = i; count_max = count_vector[i];
         } */
         if (count_mean < half_samples)  median++;
       }

    entropy = -entropy;
    brightness = sum / (half_samples*2);
    contrast = sum2 /( half_samples*2) - pow(brightness, 2);
    contrast = sqrt(contrast);

    s   = "Brightness: " + QString::number(brightness) + ".\n";
    s += "Contrast: " + QString::number(contrast) + ".\n";
    s += "Color range: [ "  + QString::number(min) +  ", " + QString::number(max) + " ].\n";
    s += "Image size: " + QString::number(width) + " (width) x " + QString::number(height) + "(height).\n";
    s += "Median: " + QString::number(median) + ".\n";
    s += "Entrophy: " + QString::number((float)entropy) + ".\n";

    textEdit->setText(s);

    textEdit->setMargin(15);



}
