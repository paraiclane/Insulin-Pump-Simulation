void QHomeWindow::updateGraph() {
    series->append(timeStep, 20); // Replace 20 with your Y-value if it's dynamic
    timeStep++;

    if (series->count() > 60) {
        series->remove(0); // Remove the first point

        // Shift all remaining points back by 1 on the X-axis
        QList<QPointF> points = series->points();
        series->clear();
        for (int i = 0; i < points.size(); ++i) {
            series->append(points[i].x() - 1, points[i].y());
        }

        timeStep = 60; // Keep timeStep at 60 so X stays in a rolling window
    }
}
