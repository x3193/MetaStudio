#version 110

uniform mat4 BoneTransform[100];	// skinning matrix

attribute vec3 Position;
attribute vec3 Normal;
attribute vec2 TexCoord;
attribute float NumBone;	// GLSL 110 converted
attribute vec4 Bone;		// GLSL 110 converted
attribute vec4 Weight;

varying vec2 texCoord;

void main()
{
	vec3 finalpos = vec3( 0.0 );
	
	// apply all weights
	for ( int i = 0; i < int( NumBone ); i++ )
	{
		finalpos += vec3( BoneTransform[ int( Bone[i] ) ] * vec4( Position, 1.0 ) ) * Weight[i];
	}
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4( finalpos, 1.0 );
	
	texCoord = TexCoord;
}
