---
--- Created by devchik10
--- DateTime: 5/16/26 3:30 AM
---
local Window = {
    title = "DEvchik10 Engine",
    width = 1920,
    height = 1080,
    isOpen = false
}

function Window.Create()
    print("[C++ Window]:'" .. Window.title .. "' [" .. Window.width .. "x" .. Window.height .. "]")
    Window.isOpen = true
end

function Init()
    print("[Lua Engine]: Инициализация..")
    Window.Create()
    print("[Lua Engine]: Готово!\n")
end

local frameCount = 0

function Update(deltaTime)

    frameCount = frameCount + 1

    print("[Engine Loop]: Кадр: " .. frameCount .. " | DeltaTime: " .. deltaTime .. " сек")

    if frameCount >= 5 then
        Window.isOpen = false
        print("\n[Engine]: Симуляция закрытия окна...")
    end
end

Init()

local currentDeltaTime = 0.016

while Window.isOpen do
    Update(currentDeltaTime)
end

print("[Lua Engine]: Работа завершена.")