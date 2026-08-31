#pragma once

class Display {
   public:
    static void Setup();
    static void ConnectToWifi(bool updating);
    static void Loop();

    static void Status(const char *message, ...);
};
