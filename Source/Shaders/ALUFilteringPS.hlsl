/*
* The ALU filtering shader
*/

Texture2D albedo : register(t0);
SamplerState sample_state : register(s0);

#include "Globals.hlsl"
#include "FilterGlobals.hlsl"

struct aniso_values
{
	float2 axis1;	
	float2 axis2;
	float p_max;
	float p_min;	
};

aniso_values compute_aniso_infos_simple(float2 duvdx, float2 duvdy, float2 duvdxs, float2 duvdys)
 {
	aniso_values values;
	
	// Side lengths of footprint and diagonals
	float px = length(duvdxs);
	float py = length(duvdys);
	
	values.axis1 = duvdx;
	values.axis2 = duvdy;
	values.p_max = max(px, py);
	values.p_min = min(px, py);
	
	return values;
 };

 aniso_values compute_aniso_infos_improved(float2 duvdx, float2 duvdy, float2 duvdxs, float2 duvdys)
 {
	aniso_values values;
	
	// Calculate diagonals
	float2 diag1 = (duvdx + duvdy) * (sqrt(2.0f)/2.0f);
	float2 diag2 = (duvdx - duvdy) * (sqrt(2.0f)/2.0f);
	float2 diags1 = (duvdxs + duvdys) * (sqrt(2.0f)/2.0f);
	float2 diags2 = (duvdxs - duvdys) * (sqrt(2.0f)/2.0f);
	
	// Side lengths of footprint and diagonals
	float px = length(duvdxs);
	float py = length(duvdys);
	float pd1 = length(diags1);	
	float pd2 = length(diags2);
	
	values.p_max = max(px, py);
	values.p_min = min(px, py);
	float pd_max = max(pd1, pd2);
	float pd_min = min(pd1, pd2);
	
	[flatten] if((values.p_max / values.p_min) < (pd_max / pd_min)) {
		values.axis1 = diag1; 
		values.axis2 = diag2;	
		values.p_max = pd_max;	
		values.p_min = pd_min;
	}
	else {
		values.axis1 = duvdx;
		values.axis2 = duvdy;
	}	
	
	return values;
 }; 

 aniso_values compute_aniso_infos_perfect(float2 duvdx, float2 duvdy, float2 duvdxs, float2 duvdys)
 {
	aniso_values values;
	const float pi_half = 1.57079f;
		
	float dudx = duvdxs[0];
	float dvdx = duvdxs[1];
	float dudy = duvdys[0];
	float dvdy = duvdys[1];
	
	// Calculate the major und minor axes of the distorted elipse inside the pixel boundaries
	float dudxsq = dudx * dudx;
	float dudysq = dudy * dudy;
	float dvdxsq = dvdx * dvdx;
	float dvdysq = dvdy * dvdy;
	
	float a = dudxsq - dudysq + dvdxsq - dvdysq;
	float b = dudxsq * dudxsq + dudysq * dudysq + dvdxsq * dvdxsq + dvdysq * dvdysq;
	float c = 2.0f * (dudysq * dudxsq + dudxsq * dvdxsq - dudxsq * dvdysq - dudysq * dvdxsq + dudysq * dvdysq + dvdysq * dvdxsq);
	float d = 8.0f * dudy * dudx * dvdy * dvdx;
	float e = 2.0f * (dudy * dudx + dvdy * dvdx);		
	
	float angle1, angle2;	
	[flatten] if(abs(dot(normalize(duvdxs), normalize(duvdys))) > 0.05f)  {
		angle1 = atan((a + sqrt(b + c + d)) / e);
		angle2 = angle1 + pi_half;
	}
	else { // Avoid numerical problems
		angle1 = 0.0f;
		angle2 = pi_half;
	}
	
	// Now that we have the angles of the axes, we just need to compute the resulting vectors
	values.axis1 = float2(-duvdx[0] * sin(angle1) - duvdy[0] * cos(angle1), -duvdx[1] * sin(angle1) - duvdy[1] * cos(angle1));
	values.axis2 = float2(-duvdx[0] * sin(angle2) - duvdy[0] * cos(angle2), -duvdx[1] * sin(angle2) - duvdy[1] * cos(angle2));

	float2 scaled_axis1 = float2(-dudx * sin(angle1) - dudy * cos(angle1), -dvdx * sin(angle1) - dvdy * cos(angle1));	
	float2 scaled_axis2 = float2(-dudx * sin(angle2) - dudy * cos(angle2), -dvdx * sin(angle2) - dvdy * cos(angle2));	
	float px = length(scaled_axis1);
	float py = length(scaled_axis2);	
	
	values.p_max = max(px, py);
	values.p_min = min(px, py);
	
	return values;
 }
 
float3 ps_main(PSInput input) : SV_Target
{
	// Get screenspace derivates of texture coords
	float2 duvdx = ddx(input.texcoord);
	float2 duvdy = ddy(input.texcoord);
	float2 duvdxs = ddx(input.scaled_texcoord);
	float2 duvdys = ddy(input.scaled_texcoord);

	aniso_values values;
	if(filter_mode == 0.0f) values = compute_aniso_infos_simple(duvdx, duvdy, duvdxs, duvdys);	
	else if(filter_mode == 1.0f) values = compute_aniso_infos_improved(duvdx, duvdy, duvdxs, duvdys);	
	else values = compute_aniso_infos_perfect(duvdx, duvdy, duvdxs, duvdys);
	
	// Calculate level of anisotropy and LOD
	float n = min(ceil((values.p_max / values.p_min) * 0.5f) * 2.0f, max_anisotropy);
	float lambda_n = min(values.p_max / values.p_min, max_anisotropy);
	float lambda = log2(values.p_max / lambda_n) + lod_bias;
	float lambda_trunc = trunc(lambda); // truncate to integer
	float trilinear_lerp_factor = lambda-lambda_trunc;	

	// Anisotropy steps  
	float2 dr = (length(values.axis1) >= length(values.axis2) ? values.axis1 : values.axis2) / n;
	float2 texcoord_start = input.texcoord - (n-1.0f) * dr * 0.5f;
	
	// NVIDIA TMU workaround
	const float lambda_tmu_fix = 0.3f;	
    
	float3 color_sum = float3(0.0f, 0.0f, 0.0f);
    
	for(float i=0.0f; i<n; ++i) {
		float2 texcoord = texcoord_start + i*dr;
		
		if(lambda <= 0.0f || !do_lerp) {
			// Magnification
			color_sum += albedo.SampleLevel(sample_state, texcoord, lambda);
		}
		else {			
			// Filter trilinear
			float3 mip1 = albedo.SampleLevel(sample_state, texcoord, lambda_trunc + lambda_tmu_fix);
			float3 mip2 = albedo.SampleLevel(sample_state, texcoord, lambda_trunc + 1.0f - lambda_tmu_fix);
		    
			color_sum += lerp(mip1, mip2, trilinear_lerp_factor);
		}
	}
	
	return (color_sum / n);
}