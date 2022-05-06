#include <platformbase/gl_interface.h>

#include <md/util/logtool.hpp>

namespace md {  
#ifndef NDEBUG
    void glCheckError(const char* cmd, const char* file, int line)
    {
        if (GLenum err = glGetError()) {
            clientmap::Log::Warning( "Error %d: %s - %s:%d", 
                err, cmd, file, line);
        }
    }
#endif
 
}  // namespace md