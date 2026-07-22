#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern "C"
{
#include "app_backend.h"
#include "stm32f4xx_hal.h"
}

Model::Model() :
    modelListener(nullptr)
{
}

void Model::tick()
{
    AppBackend_Tick(HAL_GetTick());

    AppSnapshot_t snapshot;
    AppBackend_GetSnapshot(&snapshot);

    if (modelListener != nullptr)
    {
        modelListener->onAppDataUpdated(snapshot);

        const AppUiEvent_t event =
            AppBackend_GetAndClearUiEvent();

        if (event != APP_UI_EVENT_NONE)
        {
            modelListener->onHardwareButton(event);
        }
    }
}
