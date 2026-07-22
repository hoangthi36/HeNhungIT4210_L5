#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication :
    public FrontendApplicationBase
{
public:
    FrontendApplication(
        Model& m,
        FrontendHeap& heap);

    virtual ~FrontendApplication()
    {
    }

    virtual void handleTickEvent()
    {
        model.tick();

        FrontendApplicationBase::
            handleTickEvent();
    }

    void tickModel()
    {
        model.tick();
    }

    /*
     * Các hàm navigation tự định nghĩa.
     *
     * Presenter chỉ sử dụng ba tên này,
     * không phụ thuộc tên dài TouchGFX sinh.
     */
    void gotoDashboardScreenNoTransition();

    void gotoGraphScreenNoTransition();

    void gotoSettingsScreenNoTransition();

private:
};

#endif /* FRONTENDAPPLICATION_HPP */
