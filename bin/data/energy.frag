uniform sampler2DRect srcTex;
uniform vec2 step;
uniform float time;

//varying vec2 texcoord0;

vec2 offset[9];

float kernel[9];
float kernel2[9];

float x = 0.0;
float y = 0.0;

vec4 color = vec4(0.0);
vec4 sumX = vec4(0.0);
vec4 sumY = vec4(0.0);

float luma(vec4 col){
    float l = 0.2126*col.x + 0.7152*col.g + 0.0722 * col.b;
    return l;
}

float LinearizeDepth(float zoverw){
    float n = 0.0; // camera z near
    float f = 1.0; // camera z far
    return (2.0 * n) / (f + n - zoverw * (f - n));
}

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
    kernel[3] = -2.0; kernel[4] = 0.0; kernel[5] = 2.0;
    kernel[6] = -1.0; kernel[7] = 0.0; kernel[8] = 1.0;
    
    //vertical offset
    kernel2[0] = -1.0; kernel2[1] = -2.0; kernel2[2] = -1.0;
    kernel2[3] = 0.0; kernel2[4] = 0.0; kernel2[5] = 0.0;
    kernel2[6] = 1.0; kernel2[7] = 2.0; kernel2[8] = 1.0;
    
    //calculate sobel
    /*
    for( int i = 0; i<9; i++){
        color = texture2DRect(srcTex, uv+offset[i]);
        sumX += color * kernel[i];
        
        color = texture2DRect(srcTex, uv+offset[i]);
        sumY += color * kernel2[i];
    }
    */
    // convert textures to luma
    //x = 0.2126*sumX.r + 0.7152*sumX.g + 0.0722*sumX.b;
    //y = 0.2126*sumY.r + 0.7152*sumY.g + 0.0722*sumY.b;
    
    //square luma values to denoise
    //float g = x*x + y*y;
    
    // add sobel result to srcTex
    //vec4 glowTex = texture2DRect(srcTex, uv);
    
    float sumL = luma(texture2DRect(srcTex, uv + offset[0]) + texture2DRect(srcTex, uv));
    float sumC = luma(texture2DRect(srcTex, uv + offset[1]) + texture2DRect(srcTex, uv));
    float sumR = luma(texture2DRect(srcTex, uv + offset[2]) + texture2DRect(srcTex, uv));
    
    vec3 sum;
    
    
    if(sumL < sumC && sumL < sumR){
        sum = vec3(sumL,0.0,0.0);
    } else if(sumC < sumL && sumC < sumR){
        sum = vec3(0.0,sumC,0.0);
    } else{
        sum = vec3(0.0,0.0,sumR);
    }
    
    /*
    sum = min(sumL, sumC);
    sum = min(sum, sumR);
    */
    // sine wave on the blue channel
    //glowTex += vec4(g, (g*0.5)*(sin(time*2.0)/2.0 + 0.7), g*(sin(time*2.0)/2.0 + 0.5), 1.0);
    //glowTex.rgb = clamp(glowTex.rgb, 0.0,1.0);
    gl_FragColor =vec4(vec3((1.0 - sum)),1.0);
}