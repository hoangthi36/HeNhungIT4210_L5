	#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
extern "C"
{
#include "app_backend.h"
}
class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }
    virtual void onAppDataUpdated(
            const AppSnapshot_t& snapshot) {}

    virtual void onHardwareButton(
            AppUiEvent_t event) {}

protected:
    Model* model;

};

#endif // MODELLISTENER_HPP
