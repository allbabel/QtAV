#ifndef SURFACEINTEROPDXVA_H
#define SURFACEINTEROPDXVA_H

#include <QtGlobal>

#ifdef Q_OS_WIN
#include "QtAV/SurfaceInterop.h"
#define EGL_EGLEXT_PROTOTYPES


#include <QtANGLE/EGL/egl.h>
#include <QtANGLE/EGL/eglext.h>
#include <qpa/qplatformnativeinterface.h>
#include <qtgui/qguiapplication.h>
#include <qtgui/qopenglcontext.h>
#include <QOpenGLContext>
#include <QtPlatformHeaders/QEGLNativeContext>
#include <qopenglfunctions.h>
#include <d3d9.h>
#include <dxva2api.h>
namespace QtAV
{
    class EGLWrapper;
    class Q_AV_EXPORT SurfaceInteropDXVA : public VideoSurfaceInterop
    {
    public:
        SurfaceInteropDXVA(IDirect3DDevice9 * d3device);
        ~SurfaceInteropDXVA();
        void setSurface(IDirect3DSurface9 * surface);

        virtual void* map(SurfaceType type, const VideoFormat& fmt, void* handle, int plane);
        virtual void unmap(void *handle);
        virtual void* createHandle(SurfaceType type, const VideoFormat& fmt, int plane = 0);

    private:
        IDirect3DSurface9 * _dxvaSurface;
        IDirect3DDevice9 * _d3device;
        EGLWrapper * _egl;
        GLint _glTexture;
        EGLSurface _pboSurface;
        IDirect3DTexture9 * _dxTexture;
        IDirect3DSurface9 * _dxSurface;
        EGLDisplay _eglDisplay;
        EGLConfig _eglConfig;
    };
    typedef QSharedPointer<SurfaceInteropDXVA> SurfaceInteropDXVAPtr;
}
#endif
#endif // SURFACEINTEROPDXVA_H
