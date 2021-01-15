#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 Ambient;
in vec4 Diffuse;
in vec4 Specular;
in float Shine;


uniform sampler2D boatTexture;
uniform vec3 viewPos;
uniform vec3 LightPos;
uniform vec3 LightDiffuse;
uniform vec3 LightAmbient;
uniform vec3 LightSpecular;
uniform float LightIntensity;

void main(){
	//环境光
		vec4 AmbientColor = vec4(1.0f,1.0f,1.0f,1.0f) * vec4(LightAmbient,1.0f);                     
	//漫反射                                                                                  
		float DiffuseFactor = dot(normalize(Normal), -LightPos);    
		vec4 DiffuseColor;                                                               
		if (DiffuseFactor > 0) {                                                       
			DiffuseColor = vec4(1.0f,1.0f,1.0f, 1.0f) * vec4(LightDiffuse,1.0f) *DiffuseFactor;                                              
		}                                                                              
		else {                                                                         
			DiffuseColor = vec4(0, 0, 0, 0);                                           
		}               
	// specular
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = max(0, pow(max(dot(viewDir, reflectDir), 0.0), Shine)); // when shinness is 0, 0^0 turns to 1
	vec4 specular = vec4(LightSpecular * spec *  Specular.rgb,1.0);
                                                               
	//FragColor = texture2D(boatTexture, TexCoord) * (AmbientColor + DiffuseColor);   
		

	//FragColor = vec4(ambient+diffuse+specular ,1.0);
	// // ambient
	// vec4 out_Diffuse = texture(boatTexture, TexCoord) * Diffuse;
	// vec3 ambient = LightAmbient * out_Diffuse.rgb;
	  
	// // diffuse 
	// vec3 norm = normalize(Normal);
	// vec3 lightDir = normalize(LightPos - FragPos);
	// float diff = max(dot(norm, lightDir), 0.0);
	// vec3 diffuse =LightDiffuse * diff * out_Diffuse.rgb;
	  
	// // specular
	// vec3 viewDir = normalize(viewPos - FragPos);
	// vec3 reflectDir = reflect(-lightDir, norm);  
	// float spec = max(0, pow(max(dot(viewDir, reflectDir), 0.0), Shine)); // when shinness is 0, 0^0 turns to 1
	// vec3 specular = LightSpecular * spec *  Specular.rgb; 

	//vec3 result = ambient +(diffuse +specular);
	//FragColor=vec4(ambient,1,0);
	FragColor = texture(boatTexture,TexCoord) * (AmbientColor + DiffuseColor+specular)*LightIntensity;

}


/*
   /uniform vec4 U_LightPos;			//灯光位置
uniform vec4 U_LightAmbient;		//环境光
uniform vec4 U_LightDiffuse;		//光线漫反射分量
uniform vec4 U_DiffuseMaterial;		//材质对漫反射的反射系数
uniform vec4 U_AmbientMaterial;		//模型表面材质的反射系数

varying vec4 V_Color;

varying vec3 V_Normal; 
varying vec3 V_WorldPos;			//模型受光点世界坐标轴的位置

vec4 calculate()
{
////////////////////////////////////基于点光源/////////////////////////////////////////////////
	//环境光
	vec4 color = vec4(1.0,1.0,1.0,1.0); //黑色
	float distance = 0.0;//距离d
	float constantFactor = 1.0;//常数衰减因子
	float linearFactor = 0.0;//线性衰减因子
	float quadricFactor = 0.0;//平方衰减因子

	vec4 ambientColor = Ambient * LightAmbient; //各个分量相乘
	//	color = ambientColor;

	//漫反射光3
	//vec3 lightPos = U_LightPos.xyz;

	vec3 L = lightPos - FragPos;//当前受光点的世界坐标系的位置
	distance = length(L); //获取距离

	//根据公式写出我们的光照强度的衰减因子
	float attenuation = 1.0 / (constantFactor  + linearFactor * distance + quadricFactor * quadricFactor * distance);

	//单位化
	L = normalize(L);
	//单位化
	vec3 n = normalize(Normal);
	//dot(L,n) 意思就是L的模X n的模在 cosa,当角度越小，值越量，当角度大于90得到的是一个负值，所以这里取了0 - dot(L,n)
	float diffuseIntensity = max(0.0,dot(L,n));
	vec4 diffuseColor = LightDiffuse * Diffuse * diffuseIntensity * attenuation;
	color = ambientColor + diffuseColor;

	gl_FragColor = color * V_Color;
}
*/