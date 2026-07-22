#ifndef GRAPHSCREENVIEW_HPP
#define GRAPHSCREENVIEW_HPP

#include <gui_generated/graphscreen_screen/GraphScreenViewBase.hpp>
#include <gui/graphscreen_screen/GraphScreenPresenter.hpp>

class GraphScreenView : public GraphScreenViewBase
{
public:
    GraphScreenView();
    virtual ~GraphScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void updateGraph(
        const AppSnapshot_t& snapshot);
protected:
};

#endif // GRAPHSCREENVIEW_HPP
