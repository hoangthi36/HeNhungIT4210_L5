#include <gui/graphscreen_screen/GraphScreenView.hpp>

GraphScreenView::GraphScreenView()
{
}

void GraphScreenView::setupScreen()
{
    GraphScreenViewBase::setupScreen();
}

void GraphScreenView::tearDownScreen()
{
    GraphScreenViewBase::tearDownScreen();
}

void GraphScreenView::updateGraph(
    const AppSnapshot_t& snapshot)
{
    static uint32_t counter = 0;

    counter++;

    /*
     * TouchGFX tick khoảng 60Hz.
     * Sau khoảng 30 tick thì thêm 1 điểm.
     */

    if(counter >= 30)
    {
        counter = 0;

        graphPpm.addDataPoint(snapshot.ppm);

        graphPpm.invalidate();
    }

    //---------------------------------------------------
    // Cập nhật vị trí vạch ngưỡng
    //---------------------------------------------------

    const int graphTop = graphPpm.getY();

    const int graphHeight =
        graphPpm.getHeight();

    const int graphMax = 3000;

    int yThreshold1 =
        graphTop +
        graphHeight -
        (snapshot.threshold_1 * graphHeight) /
        graphMax;

    int yThreshold2 =
        graphTop +
        graphHeight -
        (snapshot.threshold_2 * graphHeight) /
        graphMax;

    lineThreshold1.setY(yThreshold1);

    lineThreshold2.setY(yThreshold2);

    lineThreshold1.invalidate();

    lineThreshold2.invalidate();
}
