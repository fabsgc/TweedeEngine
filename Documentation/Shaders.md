# Shader file syntax

Shader files are written using JSON.

## raster
Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
fill    	  	     | wire, solid (See @te::PolygonMode)			   | @te::RASTERIZER_STATE_DESC::polygonMode
cull    	  	     | cw, ccw, none (See @te::CullingMode)			   | @te::RASTERIZER_STATE_DESC::cullMode
scissor				 | true, false				   | @te::RASTERIZER_STATE_DESC::scissorEnable
multisample			 | true, false				   | @te::RASTERIZER_STATE_DESC::multisampleEnable
lineaa				 | true, false				   | @te::RASTERIZER_STATE_DESC::antialiasedLineEnable

## depth
Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
read    	  	     | true, false				   | @te::DEPTH_STENCIL_STATE_DESC::DepthReadEnable
write    	  	     | true, false				   | @te::DEPTH_STENCIL_STATE_DESC::DepthWriteEnable
compare    	  	     | never, always, lt, lte, eq, neq, gte, gt (See @te::CompareFunction)				   | @te::DEPTH_STENCIL_STATE_DESC::DepthComparisonFunc
bias    	  	     | float				       | @te::RASTERIZER_STATE_DESC::depthBias
scaledBias    	  	 | float				       | @te::RASTERIZER_STATE_DESC::slopeScaledDepthBias
clip    	  	     | true, false				   | @te::RASTERIZER_STATE_DESC::depthClipEnable

## stencil
Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
reference    	  	 | integer			           | Reference value to use for stencil compare operations.
enabled    	  	     | true, false				   | @te::DEPTH_STENCIL_STATE_DESC::StencilEnable
readmask    	  	 | integer in [0, 255] range   | @te::DEPTH_STENCIL_STATE_DESC::StencilReadMask
writemask    	  	 | integer in [0, 255] range   | @te::DEPTH_STENCIL_STATE_DESC::StencilWriteMask
front				 | StencilOp block			   | Stencil operations and compare function for front facing geometry
back				 | StencilOp block			   | Stencil operations and compare function for back facing geometry
 
**front** and **back** options are blocks themselves, and they accept the following options:

Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
fail    	  	 	| keep, zero, replace, inc, dec, incwrap, decwrap, inverse (See @te::StencilOperation)			           | @te::DEPTH_STENCIL_STATE_DESC::frontStencilFailOp & @te::DEPTH_STENCIL_STATE_DESC::backStencilFailOp
zfail    	  	 	| keep, zero, replace, inc, dec, incwrap, decwrap, inverse (See @te::StencilOperation)			           | @te::DEPTH_STENCIL_STATE_DESC::frontStencilZFailOp & @te::DEPTH_STENCIL_STATE_DESC::backStencilZFailOp
pass    	  	 	| keep, zero, replace, inc, dec, incwrap, decwrap, inverse (See @te::StencilOperation)			           | @te::DEPTH_STENCIL_STATE_DESC::frontStencilPassOp & @te::DEPTH_STENCIL_STATE_DESC::backStencilPassOp
compare				| never, always, lt, lte, eq, neq, gte, gt (See @te::CompareFunction) | @te::DEPTH_STENCIL_STATE_DESC::frontStencilComparisonFunc & @te::DEPTH_STENCIL_STATE_DESC::backStencilComparisonFunc

## blend
Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
dither    	  	 | true, false			           | @te::BLEND_STATE_DESC::AlphaToCoverageEnable
independant    	 | true, false			           | @te::BLEND_STATE_DESC::IndependantBlendEnable
target			 | Target block					   | Blend operations for a specific render target. Multiple Target blocks can exist under a single blend block.

**Target** block accepts the following options:

Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
index    	  	     | positive integer		       | Index of the render target these options are applied to.
enabled				 | true, false				   | @te::RENDER_TARGET_BLEND_STATE_DESC::BlendEnable
writemask		     | R, G, B, A or any combination (e.g. RG, RBA, RGBA). "empty" for zero mask.					   | @te::RENDER_TARGET_BLEND_STATE_DESC::RenderTargetWriteMask
color    	  	     | BlendOp block	       	   | Represents the blend operation to execute on the color channels.
alpha				 | BlendOp block			   | Represents the blend operation to execute on the alpha channel. Only relevant if independant blend is enabled.

**BlendOp** block accepts the following options:

Name                 | Valid values				   | Reference
---------------------|---------------------------- |----------
source    	  	     | one, zero, dstRGB, srcRGB, dstIRGB, srcIRGB, dstA, srcA, dstIA, srcIA (See @te::BlendFactor)		       | @te::RENDER_TARGET_BLEND_STATE_DESC::SrcBlend, @te::RENDER_TARGET_BLEND_STATE_DESC::SrcBlendAlpha
dest    	  	     | one, zero, dstRGB, srcRGB, dstIRGB, srcIRGB, dstA, srcA, dstIA, srcIA (See @te::BlendFactor)		       | @te::RENDER_TARGET_BLEND_STATE_DESC::DstBlend, @te::RENDER_TARGET_BLEND_STATE_DESC::DstBlendAlpha
op    	  	     | add, sub, rsub, min, max (See @te::BlendOperation)		       | @te::RENDER_TARGET_BLEND_STATE_DESC::BlendOp, @te::RENDER_TARGET_BLEND_STATE_DESC::BlendOpAlpha

## GPU Program

Name                 | Valid values				  
---------------------|-------------
type                 | vertex, pixel, geometry, hull, domain, compute
language             | hlsl, glsl
compiled             | true, false
path                 | relative path to shader file
includes             | Include block

**Includes**

Name                 | Valid values				  
---------------------|-------------
type                 | sampler
name                 | sample name declared in file

## sampler

Name                 | Valid values				   				  | Reference
---------------------|--------------------------------------------|----------
name     			 | arbitrary name                             | 
addressu			 | wrap, mirror, clamp, border, mirror_once   | @te::SAMPLER_STATE_DESC::AddressMode
addressv			 | wrap, mirror, clamp, border, mirror_once   | @te::SAMPLER_STATE_DESC::AddressMode
addressw			 | wrap, mirror, clamp, border, mirror_once   | @te::SAMPLER_STATE_DESC::AddressMode
borderColor			 | float4									  | @te::SAMPLER_STATE_DESC::BorderColor
filter				 | See table below							  | @te::SAMPLER_STATE_DESC::MinFilter, @bs::SAMPLER_STATE_DESC::magFilter, @bs::SAMPLER_STATE_DESC::mipFilter
maxanisotropy		 | uint										  | @te::SAMPLER_STATE_DESC::MaxAniso
maxLOD				 | float									  | @te::SAMPLER_STATE_DESC::MipMax
minLOD				 | float									  | @te::SAMPLER_STATE_DESC::MipMin
mipLODBias			 | float									  | @te::SAMPLER_STATE_DESC::MipmapBias
comparisonfunc		 | never, less, equal, less_equal, greater, not_equal, greater_equal, always | @bs::SAMPLER_STATE_DESC::ComparisonFunc

Filter valid values:
- min_mag_mip_point
- min_mag_point_mip_linear
- min_point_mag_linear_mip_point
- min_point_mag_mip_linear
- min_linear_mag_mip_point
- min_linear_mag_point_mip_linear
- min_mag_linear_mip_point
- min_mag_mip_linear
- anisotropic
- comparison_min_mag_mip_point
- comparison_min_mag_point_mip_linear
- comparison_min_point_mag_linear_mip_point
- comparison_min_point_mag_mip_linear
- comparison_min_linear_mag_mip_point
- comparison_min_linear_mag_point_mip_linear
- comparison_min_mag_linear_mip_point
- comparison_min_mag_mip_linear
- comparison_anisotropic
- minimum_min_mag_mip_point
- minimum_min_mag_point_mip_linear
- minimum_min_point_mag_linear_mip_point
- minimum_min_point_mag_mip_linear
- minimum_min_linear_mag_mip_point
- minimum_min_linear_mag_point_mip_linear
- minimum_min_mag_linear_mip_point
- minimum_min_mag_mip_linear
- minimum_anisotropic
- maximum_min_mag_mip_point
- maximum_min_mag_point_mip_linear
- maximum_min_point_mag_linear_mip_point
- maximum_min_point_mag_mip_linear
- maximum_min_linear_mag_mip_point
- maximum_min_linear_mag_point_mip_linear
- maximum_min_mag_linear_mip_point
- maximum_min_mag_mip_linear
- maximum_anisotropic

## options

Valid options are:
Name                 | Valid values				   | Default value			| Description
---------------------|---------------------------- |------------------------|------------
separable			 | true, false				   | false					| When true, tells the renderer that passes within the shader don't need to be renderered one straight after another. This allows the system to perform rendering more optimally, but can be unfeasible for most materials which will depend on exact rendering order. Only relevant if a shader has multiple passes.
sort				 | none, backtofront, fronttoback | fronttoback			| Determines how does the renderer sort objects with this material before rendering. Most objects should be sorted front to back in order to avoid overdraw. Transparent (see below) objects will always be sorted back to front and this option is ignored. When no sorting is active the system will try to group objects based on the material alone, reducing material switching and potentially reducing CPU overhead, at the cost of overdraw.
transparent			 | true, false				   | false					| Notifies the renderer that this object is see-through. This will force the renderer to the use back to front sorting mode, and likely employ a different rendering method. Attempting to render transparent geometry without this option set to true will likely result in graphical artifacts.
forward				 | true, false				   | false					| Notifies the renderer that this object should be rendered using the forward rendering pipeline (as opposed to a deferred one).
priority			 | integer					   | 0						| Allows you to force objects with this shader to render before others. Objects with higher priority will be rendered before those with lower priority. If sorting is enabled, objects will be sorted within their priority groups (i.e. priority takes precedence over sort mode).