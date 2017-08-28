/****
 * 
 * MetaStudio is an improved MDL file format, that supports most of the features of 
 * the original MDL format. It implements skinning animation and keyframe animation.
 * Just storing little skeleton data can show very smooth animation (using quaternion 
 * for rotation interpolation).
 * This project is developed and shared by Crsky, and all explanatory rights are owned by Crsky.
 * 
 * Welcome anyone to improve this project or make suggestions to improve this project, 
 * if you can contact me, I will be very grateful to you.
 * 
 * Email : crskycode@foxmail.com
 * Date : 2017-8-29
 * 
****/

#include <stdio.h>	// printf
#include <malloc.h>		// malloc free
#include <gl/glut.h>

#include "mathlib.h"
#include "strtools.h"
#include "timetools.h"

// MetaStudio Header
#include "metastudio.h"

typedef unsigned char byte;


// Read TGA file to buffer
int LoadTGA( char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall );

// Return a time experienced after startup (second)
double get_time( void );



int GL_GenTexture( void )
{
	GLuint texnum;
	glGenTextures( 1, &texnum );
	return texnum;
}

void GL_DeleteTexture( int texnum )
{
	glDeleteTextures( 1, (GLuint *)&texnum );
}


int GL_LoadTGA( char *filename, int *width, int *height )
{
	static byte	buffer[ 1024*1024*4 ];
	int			wide;
	int			tall;
	int			texnum;

	if ( !LoadTGA( filename, buffer, sizeof( buffer ), &wide, &tall ) )
	{
		return 0;
	}

	texnum = GL_GenTexture();
	glBindTexture( GL_TEXTURE_2D, texnum );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture( GL_TEXTURE_2D, 0 );	//unbind

	if ( width )
	{
		*width = wide;
	}
	if ( height )
	{
		*height = tall;
	}

	return texnum;
}

//==============================

// Camera Setting

// rotating
// Y - up/down
// Z - left/right
float oldroty, oldrotz;
float roty, rotz;

// distance from origin
float olddist;
float dist = -30.0;	//default

// the mouse button
int mbutton = -1;

// the mouse position
float oldx, oldy;


// keyboard
int showbones;

//==============================

//================================================================================//

metastudiohdr_t			*pmetahdr;

mat4_t					bonetransform[ META_STUDIO_BONES ];
mat4_t					bonetransform_skin[ META_STUDIO_BONES ];

metastudiobodyparts_t	*pbodypart;
metastudiomodel_t		*pmodel;

vec3_t					auxverts[20000];	// in viewspace  !!! hack hack hack
vec3_t					auxnorms[20000];

int						g_time;			// current frame
int						g_body;
int						g_sequence;

//================================================================================//


void R_MetaStudioGetKeyFrame( vec3_t *pos, vec4_t *quat, metastudioanim_t *panim, int keyframe )
{
	int						i;
	metastudioanimvalue_t	*pvalue;

	pvalue = (metastudioanimvalue_t *)( (byte *)pmetahdr + panim->valueindex );

	// goto value[keyframe][0]
	pvalue += keyframe * panim->numbones;

	for ( i = 0; i < panim->numbones; i++ )
	{
		VectorCopy( pvalue[i].pos, pos[i] );
		VectorCopy4( pvalue[i].quat, quat[i] );
	}
}

void R_MetaStudioCalcRotations( vec3_t *pos, vec4_t *quat, metastudioanim_t *panim, int time )
{
	int				i, j;
	int				*pkeyframetime;
	int				starttime;
	int				endtime;
	int				curr;
	int				next;
	float			percent;

	static vec3_t	pos1[ META_STUDIO_BONES ];
	static vec4_t	quat1[ META_STUDIO_BONES ];
	static vec3_t	pos2[ META_STUDIO_BONES ];
	static vec4_t	quat2[ META_STUDIO_BONES ];

	pkeyframetime = (int *)( (byte *)pmetahdr + panim->timeindex );

	starttime = pkeyframetime[ 0 ];
	endtime = pkeyframetime[ panim->numkeyframes - 1 ];

	if ( time < starttime )
	{
		time = starttime;
	}
	else if ( time > endtime )
	{
		time = endtime;
	}

	for ( i = 0; i < panim->numkeyframes - 2; i++ )
	{
		curr = i;
		next = i + 1;

		if ( pkeyframetime[curr] == time )
		{
			R_MetaStudioGetKeyFrame( pos, quat, panim, curr );
			return;
		}
		else if ( pkeyframetime[next] == time )
		{
			R_MetaStudioGetKeyFrame( pos, quat, panim, next );
			return;
		}
		else if ( pkeyframetime[curr] < time && time < pkeyframetime[next] )
		{
			R_MetaStudioGetKeyFrame( pos1, quat1, panim, curr );
			R_MetaStudioGetKeyFrame( pos2, quat2, panim, next );

			starttime = pkeyframetime[curr];
			endtime = pkeyframetime[next];

			percent = (float)( time - starttime ) / (float)( endtime - starttime );

			for ( j = 0; j < panim->numbones; j++ )
			{
				GL_PositionSlerp( pos1[j],  pos2[j], percent, pos[j] );
				GL_QuaternionSlerp( quat1[j], quat2[j], percent, quat[j] );
			}

			return;
		}
	}
}

void R_MetaStudioSetupBones( void )
{
	int						i;
	metastudioseqdesc_t		*pseqdesc;
	metastudioanim_t		*panim;
	metastudiobone_t		*pbone;

	static vec3_t			pos[ META_STUDIO_BONES ];
	static vec4_t			quat[ META_STUDIO_BONES ];

	pbone = (metastudiobone_t *)( (byte *)pmetahdr + pmetahdr->boneindex );

	if ( g_sequence >= pmetahdr->numseq )
	{
		g_sequence = 0;

		if ( g_sequence <= 0 )
		{
			// Vertices will not make any changes
			for ( i = 0; i < pmetahdr->numbones; i++ )
			{
				MatrixCopy( pbone[i].global, bonetransform[i] );
				MatrixIdentity( bonetransform_skin[i] );
			}

			return;
		}
	}

	pseqdesc = (metastudioseqdesc_t *)( (byte *)pmetahdr + pmetahdr->seqindex ) + g_sequence;

	panim = (metastudioanim_t *)( (byte *)pmetahdr + pseqdesc->animindex );

	R_MetaStudioCalcRotations( pos, quat, panim, g_time );

	// Calculate the world transform matrix for all joints
	for ( i = 0; i < pmetahdr->numbones; i++ )
	{
		mat4_t	bonematrix;
		int		parent;

		GL_QuaternionMatrix( quat[i], bonematrix );
		VectorCopy( pos[i], bonematrix[3] );

		parent = pbone[i].parent;

		if ( parent == -1 )
		{
			MatrixCopy( bonematrix, bonetransform[i] );
			GL_ConcatTransforms( bonetransform[i], pbone[i].offset, bonetransform_skin[i] );
		}
		else
		{
			GL_ConcatTransforms( bonetransform[parent], bonematrix, bonetransform[i] );
			GL_ConcatTransforms( bonetransform[i], pbone[i].offset, bonetransform_skin[i] );
		}
	}
}

void R_MetaStudioSetupModel( int bodypart )
{
	int		index;

	if ( bodypart >= pmetahdr->numbodyparts )
	{
		bodypart = 0;
	}

	pbodypart = (metastudiobodyparts_t *)( (byte *)pmetahdr + pmetahdr->bodypartindex ) + bodypart;

	index = g_body / pbodypart->base;
	index = index % pbodypart->nummodels;

	pmodel = (metastudiomodel_t *)( (byte *)pmetahdr + pbodypart->modelindex ) + index;
}

void R_MetaStudioTransformAuxVert( int index, metastudiovertex_t *pv )
{
	int		i;
	vec3_t	temp;

	VectorClear( auxverts[index] );
	VectorClear( auxnorms[index] );

	for ( i = 0; i < pv->numbones; i++ )
	{
		GL_VectorTransform( pv->pos, bonetransform_skin[pv->bone[i]], temp );
		VectorMA( auxverts[index], pv->weight[i], temp, auxverts[index] );

		GL_VectorRotate( pv->norm, bonetransform_skin[pv->bone[i]], temp );
		VectorMA( auxnorms[index], pv->weight[i], temp, auxnorms[index] );
	}
}

void R_MetaStudioDrawPoints( void )
{
	int					i, j, k;
	metastudiovertex_t	*pvert;
	metastudiomesh_t	*pmesh;
	metastudiotexture_t	*ptexture;

	pvert = (metastudiovertex_t *)( (byte *)pmetahdr + pmodel->vertindex );

	pmesh = (metastudiomesh_t *)( (byte *)pmetahdr + pmodel->meshindex );

	ptexture = (metastudiotexture_t *)( (byte *)pmetahdr + pmetahdr->textureindex );

	// Apply all vertices to skeleton transformations
	// You can do this in the vertex shader
	for ( i = 0; i < pmodel->numverts; i++ )
	{
		R_MetaStudioTransformAuxVert( i, &pvert[i] );
	}

	// You can use VBO and VAO to make drawing faster

	for ( i = 0; i < pmodel->nummesh; i++ )
	{
		metastudiotriangle_t *ptri;
		metastudiovertex_t	 *pv;

		if ( ptexture[pmesh[i].skinref].glt )
		{
			glBindTexture( GL_TEXTURE_2D, ptexture[pmesh[i].skinref].glt );
		}
		else
		{
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		ptri = (metastudiotriangle_t *)( (byte *)pmetahdr + pmesh[i].triindex );

		glBegin( GL_TRIANGLES );

		for ( j = 0; j < pmesh[i].numtris; j++ )
		{
			int	index;

			for ( k = 0; k < 3; k++ )
			{
				index = ptri[j].indices[k];

				pv = &pvert[index];

				glTexCoord2fv( pv->uv );
				glNormal3fv( auxnorms[index] );
				glVertex3fv( auxverts[index] );
			}
		}

		glEnd();
	}
}

void R_MetaStudioDrawBones( void )
{
	int					i;
	metastudiobone_t	*pbone;

	pbone = (metastudiobone_t *)( (byte *)pmetahdr + pmetahdr->boneindex );

	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );
	glPointSize( 3.0f );
	glLineWidth( 1.0f );

	for ( i = 0; i < pmetahdr->numbones; i++ )
	{
		int	parent;

		parent = pbone[i].parent;

		if ( parent != -1 )
		{
			// draw a line from the parent joint to the current joint
			glColor3f( 0.5, 0.1, 0 );
			glBegin( GL_LINES );
			glVertex3fv( bonetransform[parent][3] );
			glVertex3fv( bonetransform[i][3] );
			glEnd();
		}

		glColor3f( 0, 1, 0 );
		glBegin( GL_POINTS );
		glVertex3fv( bonetransform[i][3] );
		glEnd();
	}
}

void R_SetupLighting( void )
{
	// For the default OpenGL coordinate system, the Y axis is upward.
	// We put the light above the origin.
	GLfloat light_position[] = { 0.0, 10.0, 0.0, 0.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_diffuse[] = { 1.8, 1.8, 1.8, 1.0 };
	GLfloat light_specular[] = { 1.5, 1.5, 1.5, 1.0 };

	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
}

void R_MetaStudioDrawModel( void )
{
	int		i;

	if ( pmetahdr->numbodyparts == 0 )
	{
		return;
	}

	R_MetaStudioSetupBones();

	//
	// Setup renderer
	//

	glEnable( GL_DEPTH_TEST );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glDepthMask( GL_TRUE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor3f( 1, 1, 1 );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	//
	// draw models
	//
	for ( i = 0; i < pmetahdr->numbodyparts; i++ )
	{
		R_MetaStudioSetupModel( i );
		R_MetaStudioDrawPoints();

		if ( showbones )
		{
			R_MetaStudioDrawBones();
		}
	}
}

void R_MetaStudioAdvanceFrame( double dt )
{
	metastudioseqdesc_t	*pseqdesc;

	if ( g_sequence >= pmetahdr->numseq )
	{
		return;
	}

	pseqdesc = (metastudioseqdesc_t *)( (byte *)pmetahdr + pmetahdr->seqindex ) + g_sequence;

	if ( dt > 0.1 )
	{
		dt = 0.1;
	}

	g_time += (int)( dt * pseqdesc->fps );

	if ( g_time > pseqdesc->duration )
	{
		// wrap
		g_time = g_time % pseqdesc->duration;
	}
}

metastudiohdr_t *Mod_LoadMetaStudioModel( char *filename )
{
	FILE				*file;
	int					size;
	byte				*buffer;
	metastudiohdr_t		*phdr;
	metastudiotexture_t	*ptexture;
	int					i;

	if ( !( file = fopen( filename, "rb" ) ) )
	{
		return 0;
	}

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek( file, 0, SEEK_SET );

	// file is too small
	if ( size < sizeof( metastudiohdr_t ) )
	{
		fclose( file );
		return 0;
	}

	buffer = malloc( size );

	if ( !buffer )
	{
		fclose( file );
		return 0;
	}

	if ( fread( buffer, size, 1, file ) != 1 )
	{
		fclose( file );
		return 0;
	}

	fclose( file );

	phdr = (metastudiohdr_t *)buffer;

	if ( phdr->id != IDSTUDIOHEADER )
	{
		free( buffer );
		return 0;
	}

	if ( phdr->version != STUDIO_VERSION )
	{
		free( buffer );
		return 0;
	}

	if ( !( phdr->flags & META_STUDIO ) )
	{
		free( buffer );
		return 0;
	}

	ptexture = (metastudiotexture_t *)( (byte *)phdr + phdr->textureindex );

	// Try loading all textures here
	for ( i = 0; i < phdr->numtextures; i++ )
	{
		char	path[260];

		strcpy( path, filename );			// "c:\mdl\1.mdl"
		StripFileName( path );				// "c:\mdl\"
		strcat( path, ptexture[i].name );	// "c:\mdl\skin.tga"

		if ( CheckExtension( path, ".tga" ) )
		{
			ptexture[i].glt = GL_LoadTGA( path, &ptexture[i].width, &ptexture[i].height );
		}
	}

	return phdr;
}

void Mod_FreeMetaStudioModel( metastudiohdr_t *phdr )
{
	int						i;
	metastudiotexture_t		*ptexture;

	ptexture = (metastudiotexture_t *)( (byte *)phdr + phdr->textureindex );

	// free all loaded textures
	for ( i = 0; i < phdr->numtextures; i++ )
	{
		if ( ptexture[i].glt )
		{
			GL_DeleteTexture( ptexture[i].glt );
		}
	}

	// free the file
	free( phdr );
}

void init( void )
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void drawAxis( void )
{
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );
	glLineWidth( 1.0f );

	glBegin( GL_LINES );
	// Red - X
	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 5.0f, 0.0f, 0.0f );
	// Green - Y
	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 5.0f, 0.0f );
	// Blue - Z
	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 5.0f );
	glEnd();
}

void display( void )
{
	static double prev;
		   double curr;
		   double dt;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	R_SetupLighting();

	// put the camera away from the origin
	glTranslatef( 0.0f, 0.0f, dist );

	// For the OpenGL's coordinate system, -Z forward, +X to the right, and +Y up.
	// For the HL's coordinate system, +X forward, -Y to the right, and +Z up.
	// To convert OpenGL's coordinate system to HL's coordinate system, just rotate it twice.
	glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
	glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );

	// Rotate HL's coordinate system
	glRotatef( rotz, 0.0f, 0.0f, 1.0f );	// left/right
	glRotatef( roty, 0.0f, 1.0f, 0.0f );	// up/down

	// Draw HL's coordinate system
	drawAxis();

	// calculate the frame interval time
	curr = get_time();
	dt = curr - prev;
	prev = curr;

	if ( pmetahdr )
	{
		R_MetaStudioAdvanceFrame( dt );
		R_MetaStudioDrawModel();
	}

	glPopMatrix();

	// Do not need to update too fast, so wait a while
	sleep( 30 );

	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape( int width, int height )
{
	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0, (GLdouble)width / (GLdouble)height, 1.0, 1000.0 );
	glMatrixMode( GL_MODELVIEW );
}

void mouse( int button, int state, int x, int y )
{
	if ( state == 0 )	// Press a key
	{
		if ( mbutton != -1 )
		{
			// already a key pressed
			return;
		}

		if ( button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON )
		{
			oldroty = roty;
			oldrotz = rotz;
			olddist = dist;
			mbutton = button;
			oldx = x;
			oldy = y;
		}
	}
	else
	{
		// The current key is released
		if ( button == mbutton )
		{
			mbutton = -1;
		}
	}
}

void motion( int x, int y )
{
	// mouse is moving

	if ( mbutton == GLUT_LEFT_BUTTON )
	{
		rotz = oldrotz + ( x - oldx );
		roty = oldroty - ( y - oldy );
	}
	else if ( mbutton == GLUT_RIGHT_BUTTON )
	{
		dist = olddist - ( y - oldy ) * 0.5;
	}
}

void keyboard( unsigned char key, int x, int y )
{
	if ( !pmetahdr )
	{
		return;
	}

	if ( key == 32 )	// Space - go to next sequence
	{
		g_time = 0;
		g_sequence++;

		if ( g_sequence >= pmetahdr->numseq )
		{
			g_sequence = 0;
		}
	}
	else if ( key == 'b' )	// B - show/hide bones
	{
		showbones = !showbones;
	}
}

int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	glutInitWindowSize( 800, 800 );
	glutCreateWindow( "Meta Studio OpenGL" );

	init();

	pmetahdr = Mod_LoadMetaStudioModel( "C:\\Users\\Crsky\\Desktop\\PV-MA_MG3\\PV-MA_MG3.mdl" );

	g_time = 0;
	g_body = 0;
	g_sequence = 0;

	showbones = 0;	//default

	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutMouseFunc( mouse );
	glutMotionFunc( motion );
	glutKeyboardFunc( keyboard );

	glutMainLoop();

	if ( pmetahdr )
	{
		Mod_FreeMetaStudioModel( pmetahdr );
	}

	return 0;
}