// Implementing several algorithms to solve the travelling salesman


#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <QTimer>
#include <queue>
#include <list>
typedef  std::pair<float, std::pair<int,int> > fii;

QVector<QPointF> v;
QGraphicsScene *scene;
std::priority_queue< fii, std::vector<fii>, std::greater<fii>  > *pq;

int pf = 20;

// Computes distance between a and b
float dist(const QPointF &a, const QPointF &b) {
    int dx = a.x() - b.x();
    int dy = a.y() - b.y();
    return dx*dx + dy*dy;
}

// Given a vector of QPoints and an index, computes the
// index of the QPoint that is closes to the v[low]
int getClosest(QVector<QPointF> &v, int low) {
    QPointF poi = v[low];
    float minDist = dist(poi, v[low + 1]);
    int minIdx = low + 1;
    for (int i = low + 2; i < v.size(); i++) {
        float d = dist(poi, v[i]);
        if (d < minDist)  {
            minDist = d;
            minIdx = i;
        }
    }
    return minIdx;
}

void advance() {
    static int i = 0;
    if (i >= v.size() - 1) return;
    int closest = getClosest(v,i);
    QPointF tmp = v[i+1];
    v[i+1] = v[closest];
    v[closest] = tmp;
    scene->addLine(v[i].x() * pf, v[i].y() * pf,
                   v[i+1].x() * pf, v[i+1].y() * pf);
    i++;
}

QVector<int> *degree;
QVector<int> *sets;

void advanceGreedy() {
    static int i = 0;
    if (i >= v.size() - 1) return;
    fii e;

    int a, b;// = e.second.first;
//    int b = e.second.second;

    do {
       e = pq->top();
       a = e.second.first;
       b = e.second.second;
       pq->pop();
    } while(degree->at(a) >= 2 || degree->at(b) >= 2 || (sets->at(a) == sets->at(b) ));


//    if ( degree->at(a) < 2 && degree->at(b) < 2  && (sets->at(a) != sets->at(b)) ) {
        (*degree)[a]++;
        (*degree)[b]++;
        int tmp = sets->at(b);
        for (int ii = 0; ii < sets->size(); ii++) {
            if (sets->at(ii) == tmp)
                (*sets)[ii] = (*sets)[a];
        }

        scene->addLine(v[a].x() * pf, v[a].y() * pf,
                       v[b].x() * pf, v[b].y() * pf);
        i++;
//    }


//    pq->pop();

}



void advanceRand() {
    static int i = 0;
    if (i >= v.size() - 1) return;
    scene->addLine(v[i].x() * pf, v[i].y() * pf,
                   v[i+1].x() * pf, v[i+1].y() * pf);
    i++;
}

void initRand() {
    std::random_shuffle(v.begin(),v.end());
}

std::list<int> *cityList;
void initNearestIntersection() {
//    cityList = new std::list<int>;
//    cityList->insert(cityList->begin(),1);
//    cityList->insert(cityList->begin(),0);
}

float totalDist(QVector<QPointF> &v, int high) {
    float d = 0;
    for (int i = 0; i < high; i++) {
        d += dist(v[i], v[i+1]);
    }
    return d;
}

int niIdx = 2;
QVector<QGraphicsLineItem *> lines;
void advanceNearestIntersection() {
//    static int i = 2;
    if (niIdx >= v.size()) return;
    float minDist = dist(v[0],v[niIdx]);
    int minIdx = niIdx;
    // find the point closest to the points
    float d;
    for (int j = 0; j < niIdx; j++) {

        for (int k = niIdx; k < v.size(); k++) {
            d = dist(v[j],v[k]);
            std::cout << "     " << j << " " << k << " " << d;
            if (d < minDist) {
                minDist = d;
                minIdx = k;
                std::cout << "<---";
            }
             std::cout << std::endl;
        }
    }

    std::cout << niIdx << " closest is:  " << minIdx << " "
            << minDist <<  std:: endl;


    // swap the closest with the last element of the vector
    QPointF tmpSwap = v[niIdx];
    QPointF closestPoint = v[minIdx];
    v[niIdx] = v[minIdx];
    v[minIdx] = tmpSwap;

    QVector<QPointF> tmpV(v.begin(), v.begin()+niIdx+1);

    tmpSwap = tmpV[niIdx];
    tmpV[niIdx] = tmpV[niIdx-1];
    tmpV[niIdx-1] = tmpSwap;
    minDist = totalDist(tmpV,niIdx);
    minIdx = niIdx - 1;
    int j = niIdx - 1;
    while (j > 1) {

        tmpSwap = tmpV[j];
        tmpV[j] = tmpV[j-1];
        tmpV[j-1] = tmpSwap;
        float d = totalDist(tmpV,niIdx);
        if (d < minDist)  {
            minDist = d;
            minIdx = j-1;
        }
        j--;

    }

    for(int j = niIdx; j > minIdx; j--) {
        v[j] = v[j-1];
    }
    v[minIdx] = closestPoint;


    for (QGraphicsLineItem *li: lines) {
        delete li;
    }
    lines.clear();
    for(int j = 0; j < niIdx - 1; j++) {
        QGraphicsLineItem *aLine = new QGraphicsLineItem(v[j].x()*pf, v[j].y()*pf,v[j+1].x()*pf, v[j+1].y()*pf );
//        scene->addLine(v[j].x()*pf, v[j].y()*pf,v[j+1].x()*pf, v[j+1].y()*pf );
        lines.push_back(aLine);
        scene->addItem(aLine);
    }
    QGraphicsLineItem *aLine = new QGraphicsLineItem(v[niIdx-1].x()*pf,
            v[niIdx-1].y()*pf,v[0].x()*pf, v[0].y()*pf );
//        scene->addLine(v[j].x()*pf, v[j].y()*pf,v[j+1].x()*pf, v[j+1].y()*pf );
    lines.push_back(aLine);
    scene->addItem(aLine);

    niIdx++;

}


void initGreedy() {
    pq = new std::priority_queue< fii, std::vector<fii>, std::greater<fii>  >;
    degree = new QVector<int>(v.size(),0);
    sets = new QVector<int>(v.size());

    for (int i = 0; i < v.size() - 1; i++) {
        for (int j = i + 1; j < v.size(); j++) {
            pq->push(fii(dist(v[i],v[j]),std::pair<int,int>(i,j)));
        }

    }
    for (int i = 0; i < sets->size(); i++) (*sets)[i] = i;
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    int maxX = 0;
    int maxY = 0;
    int pointRadius = 4;

    // QGraphicsScene *
    scene = new QGraphicsScene();

    QVector<QString> codeV;

    srand(time(NULL));
    std::ifstream inFile("/home/rarce/tmp/travelling01/us.txt");

    if (!inFile.is_open()) exit(1);

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string code;
        float x, y;
        iss >> code >> x >> y;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
        codeV.push_back(QString::fromStdString(code));
        v.push_back(QPoint(x,y));
    }

    // create an item to put into the scene
    QGraphicsRectItem * rect = new QGraphicsRectItem();
    rect->setRect(0,0,maxX * pf, maxY * pf);

    int ctr = 0;
    for (auto &p: v) {
        float tmp;
        tmp = p.x();
        p.setX(maxY - p.y() );
        p.setY(maxY - tmp);
        scene->addEllipse(p.x() * pf - pointRadius,
                          p.y() * pf - pointRadius,
                          pointRadius * 2, pointRadius * 2);
        QGraphicsTextItem *it = new QGraphicsTextItem(codeV[ctr++]);
        it->setPos(p.x() * pf, p.y() * pf);
        scene->addItem(it);
    }

    initRand();


//    initGreedy();

    initNearestIntersection();
    QTimer *timer = new  QTimer;
    QObject::connect(timer, &QTimer::timeout, advanceNearestIntersection);
    timer->start(100);

    // add a view to visualize the scene
    QGraphicsView * view = new QGraphicsView(scene);

    view->show();
    return a.exec();
}
