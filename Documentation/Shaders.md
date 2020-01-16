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
index    	  	     | positive integer		       | Index of the render target these options are applied to. If not specified the index is derived from the order in which Target blocks are defined.
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

#GPU Program

Name                 | Valid values				  
---------------------|-------------
type                 | vertex, pixel, geometry, hull, domain, compute
language             | hlsl, glsl
compiled             | true, false
path                 | relative path to shader file

~~~~~~~~~~~~~~

~~~~~~~~~~~~~~