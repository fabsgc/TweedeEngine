{
    "options":
    {
        "name": "default",
        "separable" : true,
        "sort" : "backtofront",
        "transparent" : true,
        "priority" : 100
    },
    "raster" : 
    {
        "fill" : "solid",
        "cull" : "cw",
        "scissor" : false,
        "multisample": false,
        "lineaa": false
    },
    "depth" : 
    {
        "read": true,
        "write": true,
        "compare": "less",
        "bias": 0.2,
        "scaledBias": 0.2,
        "clip": true
    },
    "stencil" : 
    {
        "enabled": true,
        "reference": 1,
        "readmask": 0,
        "writemask": 0,
        "front": 
        {
            "fail": "keep",
            "zfail": "incr",
            "pass": "keep",
            "compare": "always"
        },
        "back": 
        {
            "fail": "keep",
            "zfail": "decr",
            "pass": "keep",
            "compare": "always"
        }
    },
    "blend" : 
    {
        "dither": true,
        "independant": true,
        "targets": 
        [
            {
                "enabled": true,
                "index": 0,
                "writemask": "RGBA",
                "color": {
                    "source": "one",
                    "dest": "one",
                    "op": "add"
                },
                "alpha": {
                    "source": "one",
                    "dest": "one",
                    "op": "add"
                }
            }
        ]
    },
    "samplers": 
    [
        {
            "name" : "color",
            "addressu" : "WRAP",
            "addressv" : "WRAP",
            "addressw" : "WRAP",
            "bordercolor" : 0,
            "filter" : "MIN_MAG_MIP_POINT",
            "maxanisotropy" : 0,
            "maxlod" : 0.0,
            "minlod" : 0.0,
            "miplodbias" : 0.0,
            "compare" : "never"
        }
    ],
    "textures":
    [
        {
            "name": "texture",
            "type": "texture2d"
        }
    ],
    "programs" : 
    [
        {
            "type": "vertex",
            "language": "hlsl",
            "compiled": false,
            "version": "vs_5",
            "entry": "main",
            "path" : "Data/Shaders/Raw/Default_VS.hlsl",
            "includes": 
            [
                { "type": "sampler", "name": "color" },
                { "type": "texture", "name": "texture" }
            ]
        },
        {
            "type": "pixel",
            "language": "hlsl",
            "compiled": false,
            "version": "ps_5",
            "entry": "main",
            "path" : "Data/Shaders/Raw/Default_PS.hlsl",
            "includes": 
            [
                { "type": "sampler", "name": "color" },
                { "type": "texture", "name": "texture" }
            ]
        },
        {
            "type": "vertex",
            "language": "glsl",
            "compiled": false,
            "version": "4",
            "entry": "main",
            "path" : "Data/Shaders/Raw/Default_VS.glsl",
            "includes": 
            [
                { "type": "sampler", "name": "color" },
                { "type": "texture", "name": "texture" }
            ]
        },
        {
            "type": "pixel",
            "language": "glsl",
            "compiled": false,
            "version": "4",
            "entry": "main",
            "path" : "Data/Shaders/Raw/Default_PS.glsl",
            "includes": 
            [
                { "type": "sampler", "name": "color" },
                { "type": "texture", "name": "texture" }
            ]
        }
    ]
}