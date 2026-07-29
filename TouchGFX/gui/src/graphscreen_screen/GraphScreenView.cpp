#include <gui/graphscreen_screen/GraphScreenView.hpp>

namespace
{
const int GRAPH_MARGIN_LEFT = 8;
const int GRAPH_MARGIN_TOP = 8;
const int GRAPH_MARGIN_RIGHT = 8;
const int GRAPH_MARGIN_BOTTOM = 14;

int calculateGraphMaximum(const AppSnapshot_t& snapshot)
{
    uint32_t highest = snapshot.threshold_2;

    if (snapshot.threshold_1 > highest)
    {
        highest = snapshot.threshold_1;
    }

    if (snapshot.ppm > highest)
    {
        highest = snapshot.ppm;
    }

    /*
     * Chừa 20% khoảng trống phía trên. Với các ngưỡng rất nhỏ,
     * vẫn chừa tối thiểu 2 ppm để đường không chạm mép màn hình.
     */
    uint32_t padding = highest / 5U;
    if (padding < 2U)
    {
        padding = 2U;
    }

    highest += padding;

    if (highest < 3U)
    {
        highest = 3U;
    }

    return static_cast<int>(highest);
}

int valueToScreenY(uint16_t value,
                   int graphTop,
                   int graphHeight,
                   int rangeMaximum)
{
    if (rangeMaximum <= 0)
    {
        return graphTop + graphHeight;
    }

    uint32_t clampedValue = value;
    if (clampedValue > static_cast<uint32_t>(rangeMaximum))
    {
        clampedValue = static_cast<uint32_t>(rangeMaximum);
    }

    return graphTop + graphHeight -
           static_cast<int>((clampedValue *
                             static_cast<uint32_t>(graphHeight)) /
                            static_cast<uint32_t>(rangeMaximum));
}
}

GraphScreenView::GraphScreenView()
    : sampleTick(0U),
      graphRangeMax(0)
{
}

void GraphScreenView::setupScreen()
{
    GraphScreenViewBase::setupScreen();

    /* Bỏ 120 điểm mẫu do Designer sinh, chỉ hiển thị PPM thật. */
    graphPpm.clear();

    /* Chừa mép để cả đường PPM và hai ngưỡng không chạm viền. */
    graphPpm.setGraphAreaMargin(
        GRAPH_MARGIN_LEFT,
        GRAPH_MARGIN_TOP,
        GRAPH_MARGIN_RIGHT,
        GRAPH_MARGIN_BOTTOM);

    sampleTick = 0U;
    graphRangeMax = 0;
    graphPpm.invalidate();
}

void GraphScreenView::tearDownScreen()
{
    GraphScreenViewBase::tearDownScreen();
}

void GraphScreenView::updateGraph(
    const AppSnapshot_t& snapshot)
{
    sampleTick++;

    /*
     * TouchGFX tick khoảng 60Hz.
     * Sau khoảng 30 tick thì thêm 1 điểm.
     */

    if (sampleTick >= 30U)
    {
        sampleTick = 0U;

        graphPpm.addDataPoint(snapshot.ppm);
    }

    /*
     * Miền Y dùng chung cho graphPpm, T1 và T2. Miền này tự thay đổi
     * theo giá trị lớn nhất trong ba giá trị nên không có đường nào
     * bị đẩy khỏi màn hình khi người dùng đổi ngưỡng.
     */
    const int newGraphRangeMax =
        calculateGraphMaximum(snapshot);

    if (newGraphRangeMax != graphRangeMax)
    {
        graphRangeMax = newGraphRangeMax;
        graphPpm.setGraphRangeY(0, graphRangeMax);
    }

    const int graphTop =
        graphPpm.getY() + GRAPH_MARGIN_TOP;

    const int graphHeight =
        graphPpm.getHeight() -
        GRAPH_MARGIN_TOP -
        GRAPH_MARGIN_BOTTOM;

    const int yThreshold1 = valueToScreenY(
        snapshot.threshold_1,
        graphTop,
        graphHeight,
        graphRangeMax);

    const int yThreshold2 = valueToScreenY(
        snapshot.threshold_2,
        graphTop,
        graphHeight,
        graphRangeMax);

    /* Đường thực nằm khoảng 6 px bên trong CanvasWidget Line. */
    lineThreshold1.setY(yThreshold1 - 6);
    lineThreshold2.setY(yThreshold2 - 6);

    graphPpm.invalidate();
    lineThreshold1.invalidate();
    lineThreshold2.invalidate();
}
