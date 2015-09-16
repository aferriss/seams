//varying vec2 texcoord0;

void main(){
    gl_Position = ftransform();
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    //texcoord0 = gl_TexCoord[0].st;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}