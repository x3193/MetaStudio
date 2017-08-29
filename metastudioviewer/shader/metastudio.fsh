#version 110

uniform sampler2D Texture;

varying vec2 texCoord;

// Please implement your lighting model

void main()
{
	vec3 finalcolor = vec3( texture2D( Texture, texCoord.st ) );
	
	gl_FragColor = vec4( finalcolor, 1.0 );
}
