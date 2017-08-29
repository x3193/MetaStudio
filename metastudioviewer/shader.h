#ifndef SHADER_H
#define SHADER_H

void GL_InitShader( void );
int GL_LoadShader( char *vsh, char *fsh );

extern PFNGLBINDBUFFERARBPROC					glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC				glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC					glGenBuffersARB;
extern PFNGLBUFFERDATAARBPROC					glBufferDataARB;

extern PFNGLUSEPROGRAMOBJECTARBPROC				glUseProgramObjectARB;
extern PFNGLUNIFORM1IARBPROC					glUniform1iARB;
extern PFNGLUNIFORMMATRIX4FVARBPROC				glUniformMatrix4fvARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC			glGetUniformLocationARB;

extern PFNGLVERTEXATTRIBPOINTERARBPROC			glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC		glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC		glDisableVertexAttribArrayARB;

extern PFNGLGETATTRIBLOCATIONARBPROC			glGetAttribLocationARB;

#endif
