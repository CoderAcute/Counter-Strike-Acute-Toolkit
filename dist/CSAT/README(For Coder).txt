README(For Coder).txt

本软件的工作模式：

本软件在DLL注入阶段，创建线程，实现对D3D11交换链的Present函数的Hook，并在完成初始化后开始主循环。

在初始化阶段，按键检测线程，消息总线线程，CS2交互线程会被创建并保持独立运行。而主线程会随着D3D11的Present持续运行。

本软件架构不分UI线程（ImGui即时模式UI）和逻辑线程。子组件通过VirtualMain和Menu（+Window）的抽象，把工作逐级提交，最终在CSATiManager处汇合。所有独立线程也会受到CSATiManager的管理，借助此提供服务定位，接口管理