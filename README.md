# PanasonicSmartChinaControlCpp
> Special thanks for [mcdona1d](https://github.com/mcdona1d "github/mcdona1d") and his [Project](https://github.com/mcdona1d/panasonic_smart_china) <br>
> thanks for <https://bbs.hassbian.com/forum.php?mod=viewthread&tid=17153>
___________________________________________

This is a **API ENCAPSULATION** with **C++**.
design for Panasonic Smart China.

## Using
`
PanasonicSmartChinaControl pscc;
pscc.Init();
pscc.Login(username,password);
deviceId = pscc.GetDevice();
pscc.Set(deviceId);
`
> deviceId will in GetDevice's return
> username is your phone number
________________________________________

# WARNING

Through **Testing** , It was found This Project High Probabilty **CAN NOT** normal operation.
> If you try, good luck.