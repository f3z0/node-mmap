{
  "targets": [
    {
      "target_name": "mmap",
      "sources": ["mmap.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "MACOSX_DEPLOYMENT_TARGET": "10.12",
        "OTHER_CPLUSPLUSFLAGS": [ "-std=c++11", "-stdlib=libc++" ],
        "OTHER_LDFLAGS": [ "-stdlib=libc++" ]
      },
      "libraries": [
      ],
    }
  ]
}