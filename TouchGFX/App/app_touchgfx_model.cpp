#include <touchgfx/Application.hpp>
#include <gui/common/FrontendApplication.hpp>

/**
 * @brief Update model with latest sensor data.
 *        Called from main loop via app_touchgfx.c
 */
extern "C" void touchgfx_model_tick(void)
{
    touchgfx::Application* app =
        touchgfx::Application::getInstance();

    if (app != NULL)
    {
        FrontendApplication* frontend_app =
            static_cast<FrontendApplication*>(app);

        frontend_app->tickModel();
    }
}
