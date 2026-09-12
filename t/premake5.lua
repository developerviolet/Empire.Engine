---
--- Created by devchik10
--- DateTime: 7/8/26 12:29 AM
---

project "Platform_X11"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   staticruntime "On"
   files { "**.hpp", "**.cpp" }
   filter "system:linux"
      links { "xcb" }
   filter {}