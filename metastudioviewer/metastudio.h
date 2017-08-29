#ifndef METASTUDIO_H
#define METASTUDIO_H

// magic
#define IDSTUDIOHEADER ( ('T'<<24)+('S'<<16)+('D'<<8)+'I' )

// file version
#define STUDIO_VERSION 10

// meta studio flags
#define META_STUDIO ( 1<<8 )

// sequence flags
#define STUDIO_LOOPING	( 1<<0 )

// limits
#define META_STUDIO_BONES	256

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	int				id;				// magic number
	int				version;

	char			name[64];
	int				length;			// file size

	vec3_t			_eyeposition;
	vec3_t			_min;
	vec3_t			_max;

	vec3_t			_bmin;
	vec3_t			_bmax;

	int				flags;			// For MetaStudio, must be include META_STUDIO

	int				_numbones;
	int				_boneindex;

	int				_numbonecontrollers;
	int				_bonecontrollerindex;

	int				_numhitboxes;
	int				_hitboxindex;

	int				_numseq;
	int				_seqindex;
	
	int				_numseqgroups;
	int				_seqgroupindex;

	int				_numtextures;
	int				_textureindex;
	int				_texturedataindex;

	int				_numskinref;
	int				_numskinfamilies;
	int				_skinindex;

	int				_numbodyparts;
	int				_bodypartindex;
	
	int				_numattachments;
	int				_attachmentindex;

	int				_soundtable;
	int				_soundindex;
	int				_soundgroups;
	int				_soundgroupindex;

	int				_numtransitions;
	int				_transitionindex;

	// Meta Studio added

	int				numbones;
	int				boneindex;

	int				numhitboxes;
	int				hitboxindex;

	int				numseq;
	int				seqindex;

	int				numtextures;
	int				textureindex;

	int				numbodyparts;
	int				bodypartindex;

	int				numattachments;
	int				attachmentindex;

} metastudiohdr_t;

typedef struct
{
	char			name[32];
	int				parent;		// index to parent bone
	mat4_t			global;		// transform vector from joint-space to model-space
	mat4_t			offset;		// inverse global

} metastudiobone_t;

typedef struct
{
	int				bone;
	int				group;
	vec3_t			bmin;
	vec3_t			bmax;

} metastudiohitbox_t;

typedef struct
{
	char			label[32];

	int				flags;
	float			fps;

	int				activity;
	int				actweight;

	int				numevents;
	int				eventindex;		// metastudioevent_t events[ numevents ]

	int				duration;		// the animation ending time (last keyframe time)

	vec3_t			bmin;			// bounding box for animated vertices
	vec3_t			bmax;

	int				numblends;
	int				animindex;		// metastudioanim_t anims[ numblends ]

} metastudioseqdesc_t;

typedef struct
{
	int				frame;
	int				event;
	char			options[64];

} metastudioevent_t;

typedef struct
{
	int				numkeyframes;
	int				numbones;		// always equal to pmetahdr->numbones
	int				timeindex;		// int						time[ numkeyframes ]
	int				valueindex;		// metastudioanimvalue_t	value[ numkeyframes ][ numbones ]

} metastudioanim_t;

typedef struct
{
	vec3_t			pos;
	vec4_t			quat;

} metastudioanimvalue_t;

typedef struct
{
	char			name[64];
	int				flags;

	// for rendering
	int				width;
	int				height;
	int				glt;

} metastudiotexture_t;

typedef struct
{
	char			name[64];
	int				nummodels;
	int				base;
	int				modelindex;	// metastudiomodel_t model[ nummodels ]

} metastudiobodyparts_t;

typedef struct
{
	vec3_t			pos;
	vec3_t			tang;		// tangent space, for normal mapping
	vec3_t			bitang;
	vec3_t			norm;
	vec2_t			uv;
	int				numbones;
	int				bone[4];
	float			weight[4];

} metastudiovertex_t;

#define _META_STUDIO_MAKE_POINTER( s, m )   ( (void *) &( ((s *)0)->m ) )

#define META_STUDIO_POINTER_POSITION	_META_STUDIO_MAKE_POINTER( metastudiovertex_t, pos )
#define META_STUDIO_POINTER_TANGENT		_META_STUDIO_MAKE_POINTER( metastudiovertex_t, tang )
#define META_STUDIO_POINTER_BITANGENT	_META_STUDIO_MAKE_POINTER( metastudiovertex_t, bitang )
#define META_STUDIO_POINTER_NORMAL		_META_STUDIO_MAKE_POINTER( metastudiovertex_t, norm )
#define META_STUDIO_POINTER_TEXCOORD	_META_STUDIO_MAKE_POINTER( metastudiovertex_t, uv )
#define META_STUDIO_POINTER_NUMBONE		_META_STUDIO_MAKE_POINTER( metastudiovertex_t, numbones )
#define META_STUDIO_POINTER_BONE		_META_STUDIO_MAKE_POINTER( metastudiovertex_t, bone )
#define META_STUDIO_POINTER_WEIGHT		_META_STUDIO_MAKE_POINTER( metastudiovertex_t, weight )

typedef struct
{
	int				indices[3];

} metastudiotriangle_t;

typedef struct
{
	int				numtris;
	int				triindex;	// metastudiotriangle_t tris[ numtris ]

	int				skinref;	// index into texture array

	// rendering
	int				ebo;
	int				vao;

} metastudiomesh_t;

typedef struct
{
	char			name[64];

	int				numverts;
	int				vertindex;	// metastudiovertex_t	verts[ numverts ]

	int				nummesh;
	int				meshindex;	// metastudiomesh_t		meshes[ nummesh ]

	// rendering
	int				vbo;

} metastudiomodel_t;

typedef struct
{
	int				bone;
	vec3_t			org;

} metastudioattachment_t;

#ifdef __cplusplus
}
#endif

#endif