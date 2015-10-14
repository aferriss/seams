uniform sampler2DRect srcTex;
uniform vec2 step;
uniform float time;

//varying vec2 texcoord0;

vec2 offset[9];

float kernel[9];
float kernel2[9];
float kernel3[9];
float kernel4[9];

float x = 0.0;
float y = 0.0;
float x2 = 0.0;
float y2 = 0.0;

vec4 color = vec4(0.0);
vec4 sumX = vec4(0.0);
vec4 sumY = vec4(0.0);

vec4 sumX2 = vec4(0.0);
vec4 sumY2 = vec4(0.0);

void main(){
    
    vec2 uv = gl_TexCoord[0].st;
    
    //sample grid around pixel
    offset[0] = vec2(-step.x, -step.y); // top left
    offset[1] = vec2(0.0, -step.y); // top middle
    offset[2] = vec2(step.x, -step.y); // top right
    offset[3] = vec2(-step.x, 0.0); // middle left
    offset[4] = vec2(0.0, 0.0); //middle
    offset[5] = vec2(step.x, 0.0); //middle right
    offset[6] = vec2(-step.x, step.y); //bottom left
    offset[7] = vec2(0.0, step.y); //bottom middle
    offset[8] = vec2(step.x, step.y); //bottom right
    
    //horizontal offset
    kernel[0] = -1.0; kernel[1] = 0.0; kernel[2] = 1.0;
    kernel[3] = -1.0; kernel[4] = 0.0; kernel[5] = 1.0;
    kernel[6] = -1.0; kernel[7] = 0.0; kernel[8] = 1.0;
    
    kernel3[0] = 1.0; kernel3[1] = 0.0; kernel3[2] = -1.0;
    kernel3[3] = 1.0; kernel3[4] = 0.0; kernel3[5] = -1.0;
    kernel3[6] = 1.0; kernel3[7] = 0.0; kernel3[8] = -1.0;
    
    //vertical offset
    kernel2[0] = -1.0; kernel2[1] = -1.0; kernel2[2] = -1.0;
    kernel2[3] = 0.0; kernel2[4] = 0.0; kernel2[5] = 0.0;
    kernel2[6] = 1.0; kernel2[7] = 1.0; kernel2[8] = 1.0;
    
    kernel4[0] = 1.0; kernel4[1] = 1.0; kernel4[2] = 1.0;
    kernel4[3] = 0.0; kernel4[4] = 0.0; kernel4[5] = 0.0;
    kernel4[6] = -1.0; kernel4[7] = -1.0; kernel4[8] = -1.0;
    
    //calculate sobel
    for( int i = 0; i<9; i++){
        color = texture2DRect(srcTex, uv+offset[i]);
        sumX += color * kernel[i];
        sumY += color * kernel2[i];
        sumX2 += color * kernel3[i];
        sumY2 += color * kernel4[i];
        //color = texture2DRect(srcTex, uv+offset[i]);
        
    }
    
    // convert textures to luma
    x = 0.2126*sumX.r + 0.7152*sumX.g + 0.0722*sumX.b;
    y = 0.2126*sumY.r + 0.7152*sumY.g + 0.0722*sumY.b;
    x2 = 0.2126*sumX2.r + 0.7152*sumX2.g + 0.0722*sumX2.b;
    y2 = 0.2126*sumY2.r + 0.7152*sumY2.g + 0.0722*sumY2.b;

    //square luma values to denoise
    float g = x + y ;
    g = clamp(g, 0.0,1.0);
    float g2 = x2 + y2;
    g2 = clamp(g2, 0.0,1.0);
    g*= 2.5;
    g2*= 2.5;
    g += g2;
    g = clamp(g, 0.0,1.0);
    // add sobel result to srcTex
    //vec4 glowTex = texture2DRect(srcTex, uv);
    //g =  g;
    // sine wave on the blue channel
    //glowTex += vec4(g, (g*0.5)*(sin(time*2.0)/2.0 + 0.7), g*(sin(time*2.0)/2.0 + 0.5), 1.0);
    //glowTex.rgb = clamp(glowTex.rgb, 0.0,1.0);
    gl_FragColor =vec4(g,g,g,1.0);
}