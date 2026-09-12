# PanasonicSmartChinaControlCpp
> Special thanks for [mcdona1d](https://github.com/mcdona1d "github/mcdona1d") and his/her [Project](https://github.com/mcdona1d/panasonic_smart_china)    
> thanks for [hassbian.com](https://bbs.hassbian.com/forum.php?mod=viewthread&tid=17153)
---
# Introduce
This project is HTTP API packaging for Panasonic freah air system.
## Using
When complie,You need input your username and password on passwd.h    
Please **DO NOT** define ENABLE_CROWSERVER when you get device id.    
Using `std::cout << pscc.GetDevice()`.    
Then, **Type** deviceId on passwd.h    
passwd.h example:
```
username = "username"
password = "password"
deviceId = "1234567890AB_0000_ABCDEF-00-00"
```
complie
```bash
mkdir build && cd build && cmake .. && make -j4
```
# Interface
You can using AI for make a website.    
I make a example by AI on src/index.html, change line 199 such as 127.0.0.1:9652    
If you using Homekit, here is a [homebridge-pscc](https://github.com/realjhen123/homebridge-pscc "homebridge-pscc") build for homebridge