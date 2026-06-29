#ifdef _WIN32
#include <windows.h>
#endif

#include <glad/glad.h>

#ifdef _WIN32
static HMODULE glad_opengl32_module(void)
{
    static HMODULE module;
    if (!module) {
        module = LoadLibraryA("opengl32.dll");
    }
    return module;
}

static void *glad_get_proc(const char *name)
{
    void *proc = (void *)wglGetProcAddress(name);
    if (proc == (void *)0 || proc == (void *)1 || proc == (void *)2 || proc == (void *)3 || proc == (void *)-1) {
        HMODULE module = glad_opengl32_module();
        proc = module ? (void *)GetProcAddress(module, name) : 0;
    }
    return proc;
}
#else
static void *glad_get_proc(const char *name)
{
    (void)name;
    return 0;
}
#endif

PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
PFNGLGENBUFFERSPROC glad_glGenBuffers;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
PFNGLBINDBUFFERPROC glad_glBindBuffer;
PFNGLBUFFERDATAPROC glad_glBufferData;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
PFNGLCREATESHADERPROC glad_glCreateShader;
PFNGLSHADERSOURCEPROC glad_glShaderSource;
PFNGLCOMPILESHADERPROC glad_glCompileShader;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
PFNGLATTACHSHADERPROC glad_glAttachShader;
PFNGLLINKPROGRAMPROC glad_glLinkProgram;
PFNGLUSEPROGRAMPROC glad_glUseProgram;
PFNGLDELETESHADERPROC glad_glDeleteShader;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
PFNGLUNIFORM1FPROC glad_glUniform1f;
PFNGLUNIFORM1IPROC glad_glUniform1i;
PFNGLUNIFORM3FPROC glad_glUniform3f;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;

int gladLoadGL(void)
{
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glad_get_proc("glGenVertexArrays");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glad_get_proc("glDeleteVertexArrays");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glad_get_proc("glBindVertexArray");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC)glad_get_proc("glGenBuffers");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glad_get_proc("glDeleteBuffers");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC)glad_get_proc("glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC)glad_get_proc("glBufferData");
    glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glad_get_proc("glBufferSubData");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glad_get_proc("glEnableVertexAttribArray");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glad_get_proc("glVertexAttribPointer");
    glad_glCreateShader = (PFNGLCREATESHADERPROC)glad_get_proc("glCreateShader");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC)glad_get_proc("glShaderSource");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC)glad_get_proc("glCompileShader");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)glad_get_proc("glCreateProgram");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC)glad_get_proc("glAttachShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)glad_get_proc("glLinkProgram");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC)glad_get_proc("glUseProgram");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC)glad_get_proc("glDeleteShader");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glad_get_proc("glGetUniformLocation");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC)glad_get_proc("glUniform1f");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC)glad_get_proc("glUniform1i");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC)glad_get_proc("glUniform3f");
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glad_get_proc("glUniformMatrix4fv");
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)glad_get_proc("glActiveTexture");
    glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glad_get_proc("glGenerateMipmap");
    glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glad_get_proc("glGenFramebuffers");
    glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glad_get_proc("glDeleteFramebuffers");
    glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glad_get_proc("glBindFramebuffer");
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glad_get_proc("glCheckFramebufferStatus");
    glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glad_get_proc("glFramebufferTexture2D");
    glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glad_get_proc("glGenRenderbuffers");
    glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glad_get_proc("glDeleteRenderbuffers");
    glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)glad_get_proc("glBindRenderbuffer");
    glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glad_get_proc("glRenderbufferStorage");
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glad_get_proc("glFramebufferRenderbuffer");

    return glad_glGenVertexArrays && glad_glBindVertexArray && glad_glGenBuffers &&
           glad_glBindBuffer && glad_glBufferData && glad_glEnableVertexAttribArray &&
           glad_glVertexAttribPointer && glad_glCreateShader && glad_glCreateProgram &&
           glad_glUseProgram && glad_glActiveTexture;
}
