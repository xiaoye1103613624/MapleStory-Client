R4 first real-device run grabbed CHAR SELECT frames (not channel page).
Root cause: E:/MXD/BeiDou-Client-xy/xy-config.yaml:55 login.auto=true and Settings:102 auto=true
=> UIWorldSelect.cpp:419-439 auto branch dispatches CharlistRequestPacket(world0,ch0) at construction,
   skipping the channel page entirely; packet_dump increment proves it ([SEND] 05 00 00 00 00).
Evidence: frames vs desktop_char_select.png mean_abs 12.4..15.8 vs channel_expanded 44.9..46.2.
NOT a patch defect. Fix: temporarily set login.auto=false, restore byte-exact in finally.
