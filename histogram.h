#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QtCharts>
#include <QObject>

const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;
const int MONO = 3;

class Histogram: public QWidget
{
    Q_OBJECT
public:
    Histogram(QWidget * parent = 0);
    //Histogram(QImage*);
    Histogram(QImage*);
    void get_graph_data();
    void build_graph();
    void update_graph();

public slots:
    void set_normal() {graph_mode = true; update_graph();}
    void set_log() {graph_mode = false; update_graph();}
    void set_red() {graph_color = RED; update_graph();}
    void set_green() {graph_color = GREEN; update_graph();}
    void set_blue() {graph_color = BLUE; update_graph();}
    void set_mono() {graph_color = MONO; update_graph();}

private:
    QVBoxLayout * layout;
    QLabel* textEdit;
    QVector<QVector <unsigned long>> count_vector;
    QChartView *chartView;
    int width, height;
    QRadioButton* normal_button; QRadioButton* log_button;
    bool graph_mode = true; /*first switched to normal graph */
    int graph_color = MONO;
};

#endif // HISTOGRAM_H
