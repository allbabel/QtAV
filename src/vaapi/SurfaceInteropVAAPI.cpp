#include "SurfaceInteropVAAPI.h"
#include "utils/OpenGLHelper.h"
#include <QMutexLocker>
#include <QDebug>

namespace QtAV {
namespace vaapi {

SurfaceInteropVAAPI::SurfaceInteropVAAPI(): m_regenerateGlx(true)
{
}

SurfaceInteropVAAPI::~SurfaceInteropVAAPI()
{
    QMutexLocker lock(&m_vaapiSurfaceMutex);

    if (tmp_surfaces.isEmpty())
        return;
    QMap<GLuint*,surface_glx_ptr>::iterator it(tmp_surfaces.begin());
    while (it != tmp_surfaces.end()) {
        delete it.key();
        it.value()->destroy();
        it = tmp_surfaces.erase(it);
    }
    it = glx_surfaces.begin();
    while (it != glx_surfaces.end()) {
        it.value()->destroy();
        it = glx_surfaces.erase(it);
    }
}

surface_glx_ptr SurfaceInteropVAAPI::createGLXSurface(void *handle)
{
    GLuint tex = *((GLuint*)handle);
    surface_glx_ptr glx(new surface_glx_t());
    glx->set(m_surface);
    if (!glx->create(tex))
        return surface_glx_ptr();
    glx_surfaces[(GLuint*)handle] = glx;
    return glx;
}

void* SurfaceInteropVAAPI::map(SurfaceType type, const VideoFormat &fmt, void *handle, int plane)
{
    QMutexLocker lock(&m_vaapiSurfaceMutex);

    if (!fmt.isRGB())
        return 0;
    if (!handle) {
        handle = createHandle(type, fmt, plane);
    }
    if (type == GLTextureSurface) {

        surface_glx_ptr glx = glx_surfaces[(GLuint*)handle];

        if(!m_surface.isNull() && (m_width != m_surface->width()|| m_height != m_surface->height()))
        {
            m_width = m_surface->width();
            m_height = m_surface->height();
            m_regenerateGlx = true;
        }


        if (m_regenerateGlx || !glx) {
            if(glx) { glx.clear(); }
            glx = createGLXSurface(handle);
            m_regenerateGlx = false;
            if (!glx) {
                qWarning("Fail to create vaapi glx surface");
                return 0;
            }
        }
        glx->set(m_surface);
        if (!glx->copy())
            return 0;
        glx->sync();
        return handle;
    } else
    if (type == HostMemorySurface) {
    } else {
        return 0;
    }
    return handle;
}

void SurfaceInteropVAAPI::unmap(void *handle)
{
    QMutexLocker lock(&m_vaapiSurfaceMutex);

    QMap<GLuint*,surface_glx_ptr>::iterator it(tmp_surfaces.find((GLuint*)handle));
    if (it == tmp_surfaces.end())
        return;
    delete it.key();
    it.value()->destroy();
    tmp_surfaces.erase(it);
}

void* SurfaceInteropVAAPI::createHandle(SurfaceType type, const VideoFormat &fmt, int plane)
{
    Q_UNUSED(plane);
    if (type == GLTextureSurface) {
        if (!fmt.isRGB()) {
            return 0;
        }
        GLuint *tex = new GLuint;
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, *tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_surface->width(), m_surface->height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        tmp_surfaces[tex] = surface_glx_ptr();
        return tex;
    }
    return 0;
}

} //namespace QtAV
} //namespace vaapi

