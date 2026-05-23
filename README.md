# 🎥 tspi-camera-v4l2

![Language](https://img.shields.io/badge/language-C%2B%2B%2FC-blue)
![Framework](https://img.shields.io/badge/framework-Qt-green)
![Platform](https://img.shields.io/badge/platform-Embedded%20Linux-orange)
![V4L2](https://img.shields.io/badge/driver-V4L2-red)

基于 **V4L2** 和 **Qt** 的摄像头采集与录像应用，运行于泰山派 RK3566 嵌入式 Linux 平台，800×480 横屏显示。

> 本项目为 [tspi-camera](https://github.com/Xxx-720/tspi-camera) 的升级版，将 V4L2 采集直接集成到应用中，支持视频录制功能。

相关参考文档：
- 3.1寸屏幕移植：https://wangxingqiao.feishu.cn/wiki/U0ZLwRnn1iUJ3fkcYzZc6QA4n7f
- OV8858摄像头移植：https://wangxingqiao.feishu.cn/wiki/F1URwjDU3i6RMkkbBabcUsEpnBe

---

## ✨ 功能

- ✅ 适配泰山派 RK3566 + 3.1 寸屏幕 800×480 横屏显示
- ✅ Qt 5 嵌入式 GUI 界面，支持中文显示
- ✅ 支持开机自启，一键配置开发板运行环境
- ✅ 摄像头实时预览（NV12 格式）
- ✅ 视频录制
- ✅ 相册浏览

---

## 🛠 环境要求

| 组件 | 说明 |
|------|------|
| 开发板 | 泰山派 RK3566 |
| 系统 | Buildroot（Qt 5.12.10 交叉编译进根文件系统） |
| 显示 | Weston / Wayland |
| 驱动 | V4L2（Video for Linux 2） |
| 编译器 | gcc / g++（交叉编译工具链） |

---

## 📁 项目结构

```text
.
├── my__config/                 # 开发板一键配置文件（可直接传到板上使用）
│   ├── auto.sh                 # rcS 调用的运行脚本
│   ├── auto_config.sh          # 一键配置脚本
│   ├── rcS                     # 开机自启脚本
│   ├── weston.ini              # Weston compositor 配置文件
│   └── truetype/               # 中文字体文件
└── project/                    # Qt 相机应用源码
    ├── my_project.pro          # qmake 工程文件
    ├── main.cpp                # 🚀 程序入口
    ├── cam.cpp / cam.h         # 🖥 摄像头主界面
    ├── cam.ui                  # 🎨 UI 布局文件
    ├── camerathread.*          # 📬 摄像头采集线程
    ├── dialog.cpp/h            # 🖼 相册对话框
    ├── dialog.ui               # 🎨 相册 UI 布局
    ├── encoder.c/h             # 🎞 视频编码器
    └── v4l2_app.c/h            # 📡 V4L2 采集封装
```

---

## 🚀 快速上手（开发板端）

### 1. 准备工作

将 `my__config` 文件夹完整传输到开发板：

```bash
# 使用 adb 直接传输至板端 /wxq/my__config 路径下
adb push my__config /wxq/my__config
```

### 2. 一键配置环境

```bash
cd /wxq/my__config
chmod +x auto_config.sh
./auto_config.sh
```

脚本将自动完成：
- 配置 Weston 显示服务
- 设置中文字体
- 配置开机自启脚本
- 赋予可执行文件权限

### 3. 开机自启

脚本已自动配置，重启开发板即可自动运行程序：

```bash
reboot
```


---

## 📖 应用使用说明

### 1️⃣ 启动应用

开发板重启后自动进入相机界面，或手动执行：

```bash
cd /wxq/my__config
./my_project
```

### 2️⃣ 摄像头预览

- 应用启动后自动打开摄像头
- 实时画面显示在主窗口中央
- 画面格式为 NV12

### 3️⃣ 录制视频

- 点击 **录制按钮** 开始录像
- 再次点击停止录制，视频自动保存

### 4️⃣ 打开 / 关闭相册

- 点击 **相册按钮** 打开相册（Album）面板
- 可浏览已拍摄的照片
- 点击exit关闭相册
- 点击delete删除照片


---

## 📝 关键配置说明

### 横屏显示

通过 `weston.ini` 配置屏幕旋转：

```ini
[output]
name=DSI-1
transform=rotate-90
```

### 中文显示

`my__config/truetype/` 中包含中文字体文件，`auto_config.sh` 会自动将字体复制到系统字体目录 `/usr/share/fonts/`，Qt 程序无需额外配置即可正常显示中文。

---

## ❌ 常见问题排查

| 问题 | 排查方向 |
|------|----------|
| 程序无法启动 / 黑屏 | 显示屏移植是否正确；程序是否有可执行权限 |
| 画面无法旋转 | `weston.ini` 中 `output` 的 `name` 是否写错 |
| 中文显示乱码 | 字体文件是否已复制到 `/usr/share/fonts/`；Qt 编译时是否启用字体支持 |
| 摄像头无法打开 | V4L2 设备节点 `/dev/video*` 是否存在；权限是否正确 |

---

## 📄 License

MIT
