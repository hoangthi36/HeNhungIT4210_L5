#include <gui/graphscreen_screen/GraphScreenView.hpp>

GraphScreenView::GraphScreenView()
{
}

void GraphScreenView::setupScreen()
{
    GraphScreenViewBase::setupScreen();

    /* Bỏ 120 điểm mẫu do Designer sinh, chỉ hiển thị PPM thật. */
    graphPpm.clear();

    /* Chừa mép dưới để PPM thấp vẫn nhìn thấy đường biểu diễn. */
    graphPpm.setGraphAreaMargin(8, 8, 8, 14);
    graphPpm.invalidate();
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

    const int graphTop = graphPpm.getY() + 8;

    const int graphHeight =
        graphPpm.getHeight() - 8 - 14;

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

    lineThreshold1.setY(yThreshold1 - 5);

    lineThreshold2.setY(yThreshold2 - 5);

    lineThreshold1.invalidate();

    lineThreshold2.invalidate();
}
