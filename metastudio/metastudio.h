#ifndef METASTUDIO_H
#define METASTUDIO_H

#define STUDIO_VERSION 10
#define IDSTUDIOHEADER ( ('T'<<24)+('S'<<16)+('D'<<8)+'I' )

// meta studio flags
#define META_STUDIO ( 1<<8 )

// sequence flags
#define STUDIO_LOOPING	( 1<<0 )


typedef struct
{
	int				id;
	int				version;

	char			name[64];
	int				length;

	vec3_t			_eyeposition;
	vec3_t			_min;
	vec3_t			_max;

	vec3_t			_bmin;
	vec3_t			_bmax;

	int				flags;

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
	char			name[64];
	int				flags;
	int				width;
	int				height;
	int				index;

} _mstudiotexture_t;

typedef struct
{
	char			name[32];
	int				parent;
	mat4_t			global;
	mat4_t			offset;

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
	int				eventindex;

	int				duration;

	vec3_t			bmin;
	vec3_t			bmax;

	int				numblends;
	int				animindex;

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
	int				numbones;
	int				timeindex;
	int				valueindex;

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
	int				modelindex;

} metastudiobodyparts_t;

typedef struct
{
	vec3_t			pos;
	vec3_t			tang;
	vec3_t			bitang;
	vec3_t			norm;
	vec2_t			uv;
	int				numbones;
	int				bone[4];
	float			weight[4];

} metastudiovertex_t;

typedef struct
{
	int				indices[3];

} metastudiotriangle_t;

typedef struct
{
	int				numtris;
	int				triindex;

	int				skinref;

	// rendering
	int				vao;

} metastudiomesh_t;

typedef struct
{
	char			name[64];

	int				numverts;
	int				vertindex;

	int				nummesh;
	int				meshindex;

	// rendering
	int				vbo;

} metastudiomodel_t;

typedef struct
{
	int				bone;
	vec3_t			org;

} metastudioattachment_t;

#endif