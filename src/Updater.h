namespace Updater {
void Setup(void);
void Loop(void);
void ConnectToWifi();
bool SendOnline();
bool SendDiscovery();
bool Command(String& command, String& pay);
}  // namespace Updater
