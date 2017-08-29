#include <gl/glut.h>
#include "glext.h"
#include <stdio.h>
#include <malloc.h>

void *FS_LoadFile( char *filename )
{
	FILE	*file;
	int		size;
	char	*buffer;

	if ( !( file = fopen( filename, "rb" ) ) )
	{
		return 0;
	}

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek( file, 0, SEEK_SET );

	if ( size == 0 )
	{
		fclose( file );
		return 0;
	}

	buffer = malloc( size + 1 );	// more one for '\0'

	if ( !buffer )
	{
		fclose( file );
		return 0;
	}

	if ( fread( buffer, size, 1, file ) != 1 )
	{
		fclose( file );
		free( buffer );
		return 0;
	}

	// make sure string NULL terminated
	buffer[size] = 0;

	fclose( file );

	return buffer;
}

//===============================================================================//

// GL_ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC					glBindBufferARB;
PFNGLDELETEBUFFERSARBPROC				glDeleteBuffersARB;
PFNGLGENBUFFERSARBPROC					glGenBuffersARB;
PFNGLBUFFERDATAARBPROC					glBufferDataARB;

// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC				glDeleteObjectARB;
PFNGLDETACHOBJECTARBPROC				glDetachObjectARB;
PFNGLCREATESHADEROBJECTARBPROC			glCreateShaderObjectARB;
PFNGLSHADERSOURCEARBPROC				glShaderSourceARB;
PFNGLCOMPILESHADERARBPROC				glCompileShaderARB;
PFNGLCREATEPROGRAMOBJECTARBPROC			glCreateProgramObjectARB;
PFNGLATTACHOBJECTARBPROC				glAttachObjectARB;
PFNGLLINKPROGRAMARBPROC					glLinkProgramARB;
PFNGLUSEPROGRAMOBJECTARBPROC			glUseProgramObjectARB;
PFNGLUNIFORM1IARBPROC					glUniform1iARB;
PFNGLUNIFORMMATRIX4FVARBPROC			glUniformMatrix4fvARB;
PFNGLGETINFOLOGARBPROC					glGetInfoLogARB;
PFNGLGETUNIFORMLOCATIONARBPROC			glGetUniformLocationARB;

// GL_ARB_vertex_program
PFNGLVERTEXATTRIBPOINTERARBPROC			glVertexAttribPointerARB;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC		glEnableVertexAttribArrayARB;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	glDisableVertexAttribArrayARB;

// GL_ARB_vertex_shader
PFNGLGETATTRIBLOCATIONARBPROC			glGetAttribLocationARB;

//===============================================================================//

void GL_InitShader( void )
{
	char *exts;

	exts = (char *)glGetString( GL_EXTENSIONS );

	if ( strstr( exts, "GL_ARB_vertex_buffer_object" ) )
	{
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress( "glBindBufferARB" );
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress( "glDeleteBuffersARB" );
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress( "glGenBuffersARB" );
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress( "glBufferDataARB" );
	}

	if ( strstr( exts, "GL_ARB_shader_objects" ) )
	{
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress( "glDeleteObjectARB" );
		glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress( "glDetachObjectARB" );
		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress( "glCreateShaderObjectARB" );
		glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress( "glShaderSourceARB" );
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress( "glCompileShaderARB" );
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress( "glCreateProgramObjectARB" );
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress( "glAttachObjectARB" );
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress( "glLinkProgramARB" );
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress( "glUseProgramObjectARB" );
		glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress( "glUniform1iARB" );
		glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress( "glUniformMatrix4fvARB" );
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress( "glGetInfoLogARB" );
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress( "glGetUniformLocationARB" );
	}

	if ( strstr( exts, "GL_ARB_vertex_program" ) )
	{
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)wglGetProcAddress( "glVertexAttribPointerARB" );
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress( "glEnableVertexAttribArrayARB" );
		glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress( "glDisableVertexAttribArrayARB" );
	}

	if ( strstr( exts, "GL_ARB_vertex_shader" ) )
	{
		glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)wglGetProcAddress( "glGetAttribLocationARB" );
	}
}

int GL_CheckShader( GLhandleARB obj )
{
	GLcharARB	infolog[512];
	GLsizei		infolen;

	glGetInfoLogARB( obj, sizeof( infolog ), &infolen, infolog );

	if ( strstr( infolog, "error" ) )
	{
		printf( "%s\n", infolog );
		return 0;
	}

	if ( strstr( infolog, "ERROR" ) )
	{
		printf( "%s\n", infolog );
		return 0;
	}

	if ( strstr( infolog, "Error" ) )
	{
		printf( "%s\n", infolog );
		return 0;
	}

	return 1;
}

int GL_LoadShader( char *vsh, char *fsh )
{
	GLcharARB	*vsrc;
	GLcharARB	*fsrc;

	GLhandleARB		vert;
	GLhandleARB		frag;
	GLhandleARB		prog;

	// load shader source code
	vsrc = FS_LoadFile( vsh );
	fsrc = FS_LoadFile( fsh );

	if ( !vsrc || !fsrc )
	{
		if ( vsrc )
		{
			free( vsrc );
		}

		if ( fsrc )
		{
			free( fsrc );
		}

		// source code failed to load
		return 0;
	}

	vert = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
	frag = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

	// compile shader
	glShaderSourceARB( vert, 1, &vsrc, NULL );
	glCompileShaderARB( vert );
	glShaderSourceARB( frag, 1, &fsrc, NULL );
	glCompileShaderARB( frag );

	// free source code buffer
	free( vsrc );
	free( fsrc );

	// check if the shader has been successfully compiled
	if ( !GL_CheckShader( vert ) || !GL_CheckShader( frag ) )
	{
		glDeleteObjectARB( vert );
		glDeleteObjectARB( frag );
		return 0;
	}

	prog = glCreateProgramObjectARB();

	glAttachObjectARB( prog, vert );
	glAttachObjectARB( prog, frag );

	glLinkProgramARB( prog );

	// We have linked the shader program, and now we can delete shader objects
	glDetachObjectARB( prog, vert );	// You can delete the shader object only if the object detached from the program
	glDeleteObjectARB( vert );
	glDetachObjectARB( prog, frag );
	glDeleteObjectARB( frag );

	// check if the shader program has been successfully linked
	if ( !GL_CheckShader( prog ) )
	{
		glDeleteObjectARB( prog );
		return 0;
	}

	return prog;
}
