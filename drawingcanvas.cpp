#include "drawingcanvas.h"
#include "CustomMatrix.h"

DrawingCanvas::DrawingCanvas(QWidget *parent)  {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    /* Todo
     * Implement lines drawing per even pair
    */

    isPaintLinesClicked = true;
    update();
}

void DrawingCanvas::segmentDetection(){
    QPixmap pixmap = this->grab(); //
    QImage image = pixmap.toImage();

    cout << "image width " << image.width() << endl;
    cout << "image height " << image.height() << endl;

    //To not crash we set initial size of the matrix
    vector<CustomMatrix> windows(image.width()*image.height());

    // Get the pixel value as an ARGB integer (QRgb is a typedef for unsigned int)
    for(int i = 1; i < image.width()-1;i++){
        for(int j = 1; j < image.height()-1;j++){
            bool local_window[3][3] = {false};

            for(int m=-1;m<=1;m++){
                for(int n=-1;n<=1;n++){
                    QRgb rgbValue = image.pixel(i+m, j+n);
                    local_window[m+1][n+1] = (rgbValue != 0xffffffff);
                }
            }

            CustomMatrix mat(local_window, i, j);
            windows.push_back(mat);
        }
    }
    cout << windows.size() << endl;

    auto windowsCorrectSize = findingWindowSize(windows);
    cout << "Correct window size : " << windowsCorrectSize.size() << endl;

    reportAllFitting(image, windowsCorrectSize);
}

bool DrawingCanvas::isLineLike(const bool m[3][3]){
    int verticalCount = 0;
    int horizontalCount = 0;
    int diag1Count = 0;
    int diag2Count = 0;

    // Count pixels in each possible direction
    for (int i = 0; i < 3; i++) {
        if (m[0][i] && m[1][i] && m[2][i]) verticalCount++;
        if (m[i][0] && m[i][1] && m[i][2]) horizontalCount++;
    }

    // Diagonals
    if (m[0][0] && m[1][1] && m[2][2]) diag1Count++;
    if (m[0][2] && m[1][1] && m[2][0]) diag2Count++;

    // Consider candidate if any direction has at least 1 full or nearly full alignment
    return (verticalCount > 0 || horizontalCount > 0 || diag1Count > 0 || diag2Count > 0);
}

vector<CustomMatrix> DrawingCanvas::findingWindowSize(vector<CustomMatrix> windows){

    size_t writeIndex = 0;
    size_t removedCount = 0;

    for (size_t readIndex = 0; readIndex < windows.size(); ++readIndex) {
        if (!windows[readIndex].isAllFalse()) {
            if (writeIndex != readIndex)
                windows[writeIndex] = std::move(windows[readIndex]);
            ++writeIndex;
        } else {
            ++removedCount;
        }
    }

    windows.resize(writeIndex);

    return windows;
}

vector<CustomMatrix> DrawingCanvas::reportAllFitting(QImage image, vector<CustomMatrix> windows){
    size_t writeIndex = 0;
    if (windows.empty()){ return windows; }

    bool vertical[3][3] = {
        {0,1,0},
        {0,1,0},
        {0,1,0}
    };

    bool horizontal[3][3] = {
        {1,1,1},
        {0,0,0},
        {0,0,0}
    };

    bool diagonal1[3][3] = {
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };

    bool diagonal2[3][3] = {
        {0,0,1},
        {0,1,0},
        {1,0,0}
    };

    QPainter painter(&image);
    QPen pen(QColor(128, 0, 128));
    pen.setWidth(1);
    painter.setPen(pen);

    int matchCount = 0;

    for (size_t readIndex = 0; readIndex < windows.size(); ++readIndex) {
        if (!windows[readIndex].isPatternMatch(windows[readIndex].mat,vertical)) {
            cout << "vertical line segment at : " << windows[readIndex].originx << ", " << windows[readIndex].originy << endl;
        }
        else if (!windows[readIndex].isPatternMatch(windows[readIndex].mat,horizontal)) {
            cout << "horizontal line segment at : " << windows[readIndex].originx << ", " << windows[readIndex].originy << endl;
        }
        else if (!windows[readIndex].isPatternMatch(windows[readIndex].mat,diagonal1)) {
            cout << "lr diagonal line segment at : " << windows[readIndex].originx << ", " << windows[readIndex].originy << endl;
        }
        else if (!windows[readIndex].isPatternMatch(windows[readIndex].mat,diagonal2)) {
            cout << "rl diagonal line segment at : " << windows[readIndex].originx << ", " << windows[readIndex].originy << endl;
        }

        int rectSize = 3;
        painter.drawRect(windows[readIndex].originx - rectSize / 2, windows[readIndex].originy - rectSize / 2, rectSize, rectSize);
    }

    windows.resize(writeIndex);

    return windows;
}

vector<CustomMatrix> DrawingCanvas::automatedCandidateFinderForFree(QImage image, vector<CustomMatrix> windows){
    vector<CustomMatrix> candidates;

    QPainter painter2(this);

    QPen pen2(Qt::green, 5);
    painter2.setPen(pen2);
    painter2.setBrush(QBrush(Qt::green));

    pen2.setColor(Qt::green);
    pen2.setWidth(4); // 4-pixel wide line
    pen2.setStyle(Qt::SolidLine);
    painter2.setPen(pen2);

    for (size_t readIndex = 0; readIndex < windows.size(); ++readIndex) {
        if (isLineLike(windows[readIndex].mat)) {
            candidates.push_back(windows[readIndex]);

            // Draw candidate marker
            painter2.drawRect(windows[readIndex].originx - 2, windows[readIndex].originy - 2, 4, 4);
        }
    }

    std::cout << "Detected " << candidates.size() << " potential segment candidates.\n";
    return candidates;
}

void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up the pen and brush for drawing the points
    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    // Draw a small circle at each stored point
    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    if(isPaintLinesClicked){
        cout << "paint lines block is called" << endl;
        pen.setColor(Qt::red);
        pen.setWidth(4); // 4-pixel wide line
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        // Set the painter's pen to our custom pen.
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            //cout << m_points[i].x() << endl;
            painter.drawLine(m_points[i], m_points[i+1]);
        }
        isPaintLinesClicked = false;

        //return painter pen to blue
        pen.setColor(Qt::blue);
        painter.setPen(pen);
    }
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    // Trigger a repaint
    update();
}


