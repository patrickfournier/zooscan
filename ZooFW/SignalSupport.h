#pragma once

#include <gtk/gtk.h>
#include <functional>

namespace Zoo
{
    template<typename... TArgs>
    void InvokeGtkCallback(TArgs... args, gpointer data)
    {
        auto *cb = static_cast<std::function<void(TArgs...)> *>(data);
        (*cb)(args...);
    }

    template<typename... TArgs>
    void InvokeGtkCallbackWithParamSpecs(TArgs... args, GParamSpec *, gpointer data)
    {
        auto *cb = static_cast<std::function<void(TArgs...)> *>(data);
        (*cb)(args...);
    }

    template<typename TClass, typename... TArgs, typename TGtkType>
    void ConnectGtkSignal(TClass *obj, void (TClass::*method)(TArgs...), TGtkType widget, const char *signalName)
    {
        auto *cb = new std::function<void(TArgs...)>([obj, method](TArgs... args) {
            (obj->*method)(args...);
        });
        g_signal_connect(widget, signalName, G_CALLBACK(InvokeGtkCallback<TArgs...>), cb);
    }

    template<typename TClass, typename... TArgs, typename TGtkType>
    void ConnectGtkSignalWithParamSpecs(TClass *obj, void (TClass::*method)(TArgs...), TGtkType widget, const char *signalName)
    {
        auto *cb = new std::function<void(TArgs...)>([obj, method](TArgs... args) {
            (obj->*method)(args...);
        });
        g_signal_connect(widget, signalName, G_CALLBACK(InvokeGtkCallbackWithParamSpecs<TArgs...>), cb);
    }
}
