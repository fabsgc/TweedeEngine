{
    "raster" : 
    {
        "fill" : "solid",
        "cull" : "cw",
        "scissor" : true,
        "multisample": true,
        "lineaa": true
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
    "programs" : 
    [
        {
            "type": "vertex",
            "language": "hlsl",
            "compiled": false,
            "path" : "Data/Shaders/Raw/vertex.hlsl"
        },
        {
            "type": "pixel",
            "language": "glsl",
            "compiled": false,
            "path" : "Data/Shaders/Raw/vertex.glsl"
        },
        {
            "type": "vertex",
            "language": "hlsl",
            "compiled": false,
            "path" : "Data/Shaders/Raw/pixel.hlsl"
        },
        {
            "type": "pixel",
            "language": "glsl",
            "compiled": false,
            "path" : "Data/Shaders/Raw/pixel.glsl"
        }
    ]
}